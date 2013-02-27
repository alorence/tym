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
    _selectedRecords(selectedRecords),
    _searchPattern(searchPattern),
    _searchTerms(searchTerms),
    _dbHelper(new BPDatabase("searchTask", this)),
    _search(new SearchProvider(this)),
    _searchResultsCount(0)
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

        if(_selectedRecords.count() != _searchTerms.count()) {
            LOG_ERROR("Number of library entries selected and number of search terms does not match...");
            emit finished();
            return;
        }

        QMap<QString, QString> *searchMap = new QMap<QString, QString>();
        for(int i = 0 ; i < _selectedRecords.count() ; ++i) {
            (*searchMap)[_selectedRecords.value(i).value(Library::Uid).toString()] = _searchTerms.value(i);
        }

        _searchResultsCount += searchMap->count();
        _search->searchManually(searchMap);

    } else {
        QMap<QString, QString> *bpidSearchMap = new QMap<QString, QString>();
        QMap<QString, QString> *fullInfosSearchMap = new QMap<QString, QString>();

        FileBasenameParser bpidParser(TYM_BEATPORT_DEFAULT_FORMAT);
        FileBasenameParser fullInfosParser(_searchPattern);
        foreach (QSqlRecord record, _selectedRecords) {

            QString uid = record.value(Library::Uid).toString();
            QString baseName = QFileInfo(record.value(Library::FilePath).toString()).baseName();

            if(bpidParser.hasMatch(baseName)) {
                (*bpidSearchMap)[uid] = bpidParser.parse(baseName)[TrackFullInfos::Bpid];
            } else if(fullInfosParser.hasMatch(baseName)) {
                // Keep track of parsing result, to use it in selectBetterResult()
                _trackParsedInformation[uid] = fullInfosParser.parse(baseName);
                (*fullInfosSearchMap)[uid] = ((QStringList)_trackParsedInformation[uid].values()).join(' ');
            } else {
                LOG_INFO(tr("Unable to extract information from %1 file").arg(baseName));
            }
        }

        _dbHelper->dbObject().transaction();
        if( ! bpidSearchMap->empty()) {
            _searchResultsCount += 1;
            _search->searchFromIds(bpidSearchMap);
        }
        if( ! fullInfosSearchMap->empty()) {
            _searchResultsCount += fullInfosSearchMap->count();
            _search->searchManually(fullInfosSearchMap);
        }
    }
}

void SearchTask::checkCountResults()
{
    --_searchResultsCount;
    if(_searchResultsCount <= 0) {
        if( ! _searchPattern.isNull()) {
            selectBetterResult();
        }

        _dbHelper->dbObject().commit();
        emit finished();
    }
}

void SearchTask::selectBetterResult() const
{

    QRegularExpression stringPurifyRegexp("[-\\[\\](),&'_ +?]");

    foreach (QSqlRecord record, _selectedRecords) {
        QString uid = record.value(Library::Uid).toString();

        QSqlQuery results = _dbHelper->resultsForTrack(uid);

        // This map link each result (with its bpid) with a score, to find the better one
        QMap<QString, int> resultScore;
        while(results.next()) {
            QSqlRecord result = results.record();
            int score = 0;

            QMapIterator<TrackFullInfos::Indexes, QString> it(_trackParsedInformation[uid]);

            // Check for each parsed information, for a corresponding result
            while(it.hasNext()) {
                TrackFullInfos::Indexes index = it.next().key();

                // To ensure minor differences will not affect the string compare
                QString value = it.value();
                value = value.remove(stringPurifyRegexp).normalized(QString::NormalizationForm_D);
                QString resultValue = result.value(index).toString().remove(stringPurifyRegexp).normalized(QString::NormalizationForm_D);

                if(value.compare(resultValue, Qt::CaseInsensitive) == 0) {
                    ++score;
                } else {
                    qDebug() << value << resultValue << value.compare(resultValue, Qt::CaseInsensitive);
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
    }
}

