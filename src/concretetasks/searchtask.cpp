/*****************************************************************************
Copyright 2013 Antoine Lorence. All right reserved.

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
#include "tools/searchprovider.h"
#include "dbaccess/bpdatabase.h"

SearchTask::SearchTask(const QList<QSqlRecord> &selectedRecords, const QString &searchPattern, const QStringList &searchTerms, QObject *parent) :
    Task(parent),
    _searchPattern(searchPattern),
    _selectedRecords(selectedRecords),
    _searchTerms(searchTerms),
    _dbHelper(new BPDatabase("searchTask", this)),
    _search(new SearchProvider(this)),
    _numTracksToUpdate(0)
{

    connect(_search, &SearchProvider::searchResultAvailable, _dbHelper, &BPDatabase::storeSearchResults);

    connect(_dbHelper, &BPDatabase::searchResultStored, this, &SearchTask::checkCountResults);
}

SearchTask::~SearchTask()
{
    delete _dbHelper;
    delete _search;
}

void SearchTask::run()
{
    LOG_TRACE(tr("Start search task"));

    if( ! _searchTerms.isEmpty()) {

        emit initializeProgression(1);

        if(_selectedRecords.count() != _searchTerms.count()) {
            LOG_ERROR(tr("Number of library entries selected and number of search terms does not match..."));
            emit finished();
            return;
        }

        auto searchMap = new QMap<QString, QString>();
        for(int i = 0 ; i < _selectedRecords.count() ; ++i) {
            (*searchMap)[_selectedRecords.value(i).value(Library::Uid).toString()] = _searchTerms.value(i);
        }

        _numTracksToUpdate += searchMap->count();
        _search->searchManually(searchMap);

    } else {
        auto bpidSearchMap = new QMap<QString, QString>();
        auto fullInfosSearchMap = new QMap<QString, QString>();

        int numTracksToAutoLink = 0;

        FileBasenameParser bpidParser(TYM_BEATPORT_DEFAULT_FORMAT);
        FileBasenameParser fullInfosParser(_searchPattern);
        for(QSqlRecord record : _selectedRecords) {

            QString uid = record.value(Library::Uid).toString();
            QString baseName = QFileInfo(record.value(Library::FilePath).toString()).completeBaseName();

            if(bpidParser.hasMatch(baseName)) {
                (*bpidSearchMap)[uid] = bpidParser.parse(baseName)[TrackFullInfos::Bpid];
                ++_numTracksToUpdate;
            } else if(fullInfosParser.hasMatch(baseName)) {
                // Keep track of parsing result, to use it in selectBetterResult()
                _trackParsedInformation[uid] = fullInfosParser.parse(baseName);
                (*fullInfosSearchMap)[uid] = ((QStringList)_trackParsedInformation[uid].values()).join(' ');
                ++_numTracksToUpdate;
                ++numTracksToAutoLink;
            } else {
                LOG_INFO(tr("Unable to extract information from %1 file").arg(baseName));
            }
        }

        emit initializeProgression(_numTracksToUpdate * 3 + numTracksToAutoLink);

        _dbHelper->dbObject().transaction();
        if( ! bpidSearchMap->empty()) {
            _search->searchFromIds(bpidSearchMap);
        }
        if( ! fullInfosSearchMap->empty()) {
            _search->searchManually(fullInfosSearchMap);
        }
    }
}

void SearchTask::checkCountResults()
{
    --_numTracksToUpdate;
    increaseProgressStep(3);

    if(_numTracksToUpdate <= 0) {
        if( ! _searchPattern.isNull()) {
            selectBetterResult();
        }

        _dbHelper->dbObject().commit();
        emit finished();
    }
}

void SearchTask::selectBetterResult()
{

    QRegularExpression stringPurifyRegexp("[-\\[\\](),&'_ +?]");

    for(QSqlRecord record : _selectedRecords) {
        QString uid = record.value(Library::Uid).toString();

        QSqlQuery results = _dbHelper->resultsForTrack(uid);

        // This map link each result (with its bpid) with a score, to find the better one
        QMap<QString, int> resultScore;
        while(results.next()) {
            QSqlRecord result = results.record();
            int score = 0;

            QMapIterator<TrackFullInfos::TableIndexes, QString> it(_trackParsedInformation[uid]);

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
        increaseProgressStep(1);

    }
}

