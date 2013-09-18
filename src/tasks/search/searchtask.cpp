/*****************************************************************************
Copyright 2012-2013 Antoine Lorence. All right reserved.

This file is part of TYM (Tag Your Music).

TYM (Tag Your Music) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TYM (Tag Your Music) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TYM (Tag Your Music). If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "searchtask.h"

#include "tools/patterntool.h"
#include "network/searchprovider.h"
#include "dbaccess/bpdatabase.h"

#define SEARCH_DURATION       3
#define DB_STORE_DURATION     4
#define LINK_RESULT_DURATION  1

SearchTask::SearchTask(const QList<QSqlRecord> &libraryRecords, QObject *parent) :
    Task(parent),
    _libraryRecords(libraryRecords),
    _nbResponseExpected(0),
    _dbHelper(new BPDatabase("searchTask", this)),
    _searchProvider(new SearchProvider(this))
{
    connect(_searchProvider, &SearchProvider::searchResultAvailable,
            this, &SearchTask::updateLibraryWithResults);
    connect(_searchProvider, &SearchProvider::searchResultAvailable,
            this, &SearchTask::startDbStoringTask);
}

SearchTask::~SearchTask()
{
    delete _dbHelper;
    delete _searchProvider;
}

void SearchTask::setSearchFromId(bool enabled)
{
    _bpidSearchEnabled = enabled;
}

void SearchTask::setNaiveSearch(bool enabled)
{
    _naiveSearchEnabled = enabled;
}

void SearchTask::setManualSearch(bool enabled, QMap<QString, QString> searchTerms)
{
    _manualSearchEnabled = enabled;
    _searchTerms = searchTerms;
}

void SearchTask::enableBetterResultDetection(bool enabled)
{
    _betterResultDetection = enabled;
}

void SearchTask::run()
{
    LOG_TRACE(QString("Start search task"));

    auto bpidSearchMap = new QMap<QString, QString>();
    auto naiveSearchMap = new QMap<QString, QString>();
    auto manualSearchMap = new QMap<QString, QString>();

    _nbResponseExpected = 0;

    FileBasenameParser bpidParser(TYM_BEATPORT_DEFAULT_FORMAT);

    for(QSqlRecord record : _libraryRecords) {

        // Compute information used in all searches types
        QString uid = record.value(Library::Uid).toString();
        QFileInfo file(record.value(Library::FilePath).toString());
        QString baseName = file.completeBaseName();

        uint savePreviousNbResponse = _nbResponseExpected;

        if(_bpidSearchEnabled) {
            // Bpid parser extract information from track filename
            if(bpidParser.hasMatch(baseName)) {
                // We use only the Beatport ID of the track
                bpidSearchMap->insert(uid, bpidParser.parse(baseName)[TrackFullInfos::Bpid]);
                ++_nbResponseExpected;
            }

        }

        if(_naiveSearchEnabled) {
            // Search is simply done from track filename
            naiveSearchMap->insert(uid, baseName);
            ++_nbResponseExpected;
        }

        if(_manualSearchEnabled) {
            // We use text typed by user as base for the search request
            QString searchQuery = _searchTerms.value(uid);
            if(!searchQuery.isEmpty()) {
                manualSearchMap->insert(uid, searchQuery);
                ++_nbResponseExpected;
            }
        }

        // If we expect new results for this track, notify the monitor
        if(savePreviousNbResponse < _nbResponseExpected) {
            emit newSubResultElement(uid, file.fileName());
        }
    }


    // Configure prograssBar in the monitor
    uint totalSteps = _nbResponseExpected * (SEARCH_DURATION+DB_STORE_DURATION);
    if(_betterResultDetection) {
        totalSteps += _libraryRecords.count() * LINK_RESULT_DURATION;
    }
    emit initializeProgression(totalSteps);

    emit currentStatusChanged(tr("Send search requests to Beatport"));
    if( ! bpidSearchMap->empty()) {
        _searchProvider->beatportIdsBasedSearch(bpidSearchMap);
    }
    if( ! naiveSearchMap->empty()) {
        _searchProvider->naturalSearch(naiveSearchMap);
    }
    if( ! manualSearchMap->empty()) {
        _searchProvider->naturalSearch(manualSearchMap);
    }
    _dbHelper->dbObject().transaction();
}

void SearchTask::updateLibraryWithResults(QString libId, QJsonValue result)
{
    int nbResults = result.toArray().size();
    if(nbResults == 0) {
        emit subResultAvailable(libId, Utils::Warning, tr("No new result appended"));
    } else {
        emit subResultAvailable(libId, Utils::Info, tr("%1 new result(s) appended").arg(nbResults));
    }

    increaseProgressStep(SEARCH_DURATION);
    _dbHelper->storeSearchResults(libId, result);
    increaseProgressStep(DB_STORE_DURATION);

    // We reached the last result of all requests sent
    if(--_nbResponseExpected <= 0) {

        if(_betterResultDetection) {
            emit currentStatusChanged(tr("Try to detect the better result for each file"));
            for(QSqlRecord record : _libraryRecords) {
                selectBetterResult(record);
            }
            emit finished(tr("You can now check your tracks and verify the results automatically "
                             "set is the better one."));
        } else {
            emit finished(tr("Search is now done for all your tracks. You need to check results "
                             "and verify which is the better for each of your tracks."));
        }
        _dbHelper->dbObject().commit();
    }
}

void SearchTask::startDbStoringTask()
{
    disconnect(_searchProvider, &SearchProvider::searchResultAvailable,
            this, &SearchTask::startDbStoringTask);
    emit currentStatusChanged(tr("Store results in database"));
}

void SearchTask::selectBetterResult(const QSqlRecord &record)
{
    // Used to retrieve user preferred patterns
    QSettings settings;
    QStringList patterns = settings.value(TYM_PATH_PATTERNS, TYM_DEFAULT_PATTERNS).toStringList();

    if(patterns.empty()) {
        // Error message, we can't do nothing
    }

    // Used to remove some characters when comparing and scoring results
    QRegularExpression stringPurifyRegexp("[-\\[\\](),&'_ +?]");
    // used to extract information from filename
    FileBasenameParser parser;

    // information about the current track to analyze
    QString uid = record.value(Library::Uid).toString();
    QFileInfo file(record.value(Library::FilePath).toString());

    // For each pattern defined by User
    for(QString pattern : patterns) {
        parser.setPattern(pattern);

        if(parser.hasMatch(file.completeBaseName())) {
            QSqlQuery results = _dbHelper->resultsForTrack(uid);
            auto parsedContent = parser.parse(file.completeBaseName());

            // FIXME: try to limit bad behavior on this method
            // ========= Old algorythm starts here ===========

            // This map link each result (with its bpid) with a score, to find the better one
            QMap<QString, int> resultScore;
            while(results.next()) {
                QSqlRecord result = results.record();
                int score = 0;

                QMapIterator<TrackFullInfos::TableIndexes, QString> it(parsedContent);

                // Check for each parsed information, for a corresponding result
                while(it.hasNext()) {
                    TrackFullInfos::TableIndexes index = it.next().key();

                    // To ensure minor differences will not affect the string compare
                    QString value = it.value();
                    value = value.remove(stringPurifyRegexp).normalized(QString::NormalizationForm_D);
                    QString resultValue = result.value(index).toString().remove(stringPurifyRegexp)
                            .normalized(QString::NormalizationForm_D);

                    if(value.compare(resultValue, Qt::CaseInsensitive) == 0) {
                        ++score;
                    }
                }

                resultScore[result.value(TrackFullInfos::Bpid).toString()] = score;
            }

            QMapIterator<QString, int> resultScoreIt(resultScore);
            QString betterResult = "";
            int betterScore = 0;
            while(resultScoreIt.hasNext()) {
                if(resultScoreIt.next().value() > betterScore) {
                    betterScore = resultScoreIt.value();
                    betterResult = resultScoreIt.key();
                }
            }

            if(betterScore > 0) {
                QSqlRecord track = _dbHelper->trackInformations(betterResult);
                QString trackString = track.value(TrackFullInfos::Artists).toString() + " - "
                        + track.value(TrackFullInfos::Title).toString();
                emit subResultAvailable(uid, Utils::Success, tr("Better result found: %1").arg(trackString));
                _dbHelper->setLibraryTrackReference(uid, betterResult);
            } else {
                emit subResultAvailable(uid, Utils::Error, tr("No good result found"));
            }
            increaseProgressStep(LINK_RESULT_DURATION);
        }
    }
}

