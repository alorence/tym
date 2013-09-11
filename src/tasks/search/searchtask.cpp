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

SearchTask::SearchTask(const QList<QSqlRecord> &selectedRecords, QObject *parent) :
    Task(parent),
    _selectedRecords(selectedRecords),
    _nbResultsToReceive(0),
    _dbHelper(new BPDatabase("searchTask", this)),
    _searchProvider(new SearchProvider(this))
{
    connect(_searchProvider, &SearchProvider::searchResultAvailable,
            this, &SearchTask::updateLibraryWithResults);
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

void SearchTask::setAutomaticSearch(bool enabled, const QString &pattern)
{
    _autoSearchEnabled = enabled;
    _searchPattern = pattern;
}

void SearchTask::setManualSearch(bool enabled, QMap<QString, QString> searchTerms)
{
    _manualSearchEnabled = enabled;
    _searchTerms = searchTerms;
}

void SearchTask::run()
{
    LOG_TRACE(tr("Start search task"));

    auto bpidSearchMap = new QMap<QString, QString>();
    auto fullInfosSearchMap = new QMap<QString, QString>();
    auto manualSearchMap = new QMap<QString, QString>();

    _nbResultsToReceive = 0;

    if(_bpidSearchEnabled) {

        FileBasenameParser parser(TYM_BEATPORT_DEFAULT_FORMAT);

        for(QSqlRecord record : _selectedRecords) {
            QString uid = record.value(Library::Uid).toString();
            QString baseName = QFileInfo(record.value(Library::FilePath).toString()).completeBaseName();

            if(parser.hasMatch(baseName)) {
                bpidSearchMap->insert(uid, parser.parse(baseName)[TrackFullInfos::Bpid]);

                ++_nbResultsToReceive;
            }
        }
    }

    if(_autoSearchEnabled) {

        FileBasenameParser parser(_searchPattern);

        for(QSqlRecord record : _selectedRecords) {
            QString uid = record.value(Library::Uid).toString();
            QString baseName = QFileInfo(record.value(Library::FilePath).toString()).completeBaseName();

            if(parser.hasMatch(baseName)) {
                auto parseResult = parser.parse(baseName);
                _trackParsedInformation.insert(uid, parseResult);
                fullInfosSearchMap->insert(uid, ((QStringList) parseResult.values()).join(' '));

                ++_nbResultsToReceive;
            }
        }

    }

    if(_manualSearchEnabled) {

        for(QString uid : _searchTerms.keys()) {
            QString searchTerm = _searchTerms[uid];
            if(!searchTerm.isEmpty()) {
                manualSearchMap->insert(uid, searchTerm);
                ++_nbResultsToReceive;
            }
        }
    }

    // Configure prograssBar in the monitor
    emit initializeProgression(_nbResultsToReceive * (SEARCH_DURATION+DB_STORE_DURATION)
                               + _trackParsedInformation.count() * LINK_RESULT_DURATION);

    if( ! bpidSearchMap->empty()) {
        _searchProvider->searchFromIds(bpidSearchMap);
    }
    if( ! fullInfosSearchMap->empty()) {
        _searchProvider->searchManually(fullInfosSearchMap);
    }
    if( ! manualSearchMap->empty()) {
        _searchProvider->searchManually(manualSearchMap);
    }
    _dbHelper->dbObject().transaction();
}

void SearchTask::updateLibraryWithResults(QString libId, QJsonValue result)
{
    increaseProgressStep(SEARCH_DURATION);
    _dbHelper->storeSearchResults(libId, result);
    increaseProgressStep(DB_STORE_DURATION);

    if(--_nbResultsToReceive <= 0) {
        for(QString libId : _trackParsedInformation.keys()) {
            selectBetterResult(libId, _trackParsedInformation[libId]);
        }
        _dbHelper->dbObject().commit();
        emit finished();
    }
}

// TODO: try to limit bad behavior on this method
void SearchTask::selectBetterResult(const QString &uid,
                                    QMap<TrackFullInfos::TableIndexes, QString> parsedContent)
{
    QRegularExpression stringPurifyRegexp("[-\\[\\](),&'_ +?]");

    QSqlQuery results = _dbHelper->resultsForTrack(uid);

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
            QString resultValue = result.value(index).toString().remove(stringPurifyRegexp).normalized(QString::NormalizationForm_D);

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
        _dbHelper->setLibraryTrackReference(uid, betterResult);
    }
    increaseProgressStep(LINK_RESULT_DURATION);
}

