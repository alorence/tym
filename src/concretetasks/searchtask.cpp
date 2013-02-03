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

#include "commons.h"
#include "tools/patterntool.h"
#include "tools/searchprovider.h"
#include "dbaccess/bpdatabase.h"

SearchTask::SearchTask(QString searchPattern, SearchWizard::SearchType searchType, QList<QSqlRecord> selectedRecords, QObject *parent) :
    Task(parent),
    _searchPattern(searchPattern),
    _searchType(searchType),
    _selectedRecords(selectedRecords),
    _dbHelper(new BPDatabase("searchTask", this)),
    _search(new SearchProvider(this)),
    _searchResultsCount(0)
{
    connect(_search, SIGNAL(searchResultAvailable(QString,QJsonValue)),
            _dbHelper, SLOT(storeSearchResults(QString,QJsonValue)));

    connect(_dbHelper, SIGNAL(searchResultStored(QString)), this, SLOT(checkCountResults()));
}

SearchTask::~SearchTask()
{
    delete _dbHelper;
    delete _search;
}

void SearchTask::run()
{
    LOG_TRACE("Start search task");
    QMap<QString, QMap<TrackFullInfos::Indexes, QString> > fileInformations;

    FileBasenameParser fileParser(_searchPattern);
    foreach (QSqlRecord record, _selectedRecords) {
        QFileInfo file(record.value(Library::FilePath).toString());

        fileInformations[record.value(Library::Uid).toString()] = fileParser.parse(file.baseName());
    }

    _dbHelper->dbObject().transaction();
    QMap<QString, QString> * requestMap = new QMap<QString, QString>();
    if(_searchType == SearchWizard::FromId) {
        foreach(QString libUid, fileInformations.keys()) {
            QString bpid = fileInformations[libUid][TrackFullInfos::Bpid];
            if(bpid.isEmpty()) continue;

            (*requestMap)[libUid] = bpid;
        }
        if( ! requestMap->isEmpty()) {
            _searchResultsCount = 1;
            _search->searchFromIds(requestMap);
        }
    } else {
        foreach(QString libUid, fileInformations.keys()) {
            if(fileInformations[libUid].values().isEmpty()) continue;

            // TODO: Add to requestMap only informations selected by user

            (*requestMap)[libUid] = ((QStringList) fileInformations[libUid].values()).join(" ");
        }
        if( ! requestMap->isEmpty()) {
            _searchResultsCount = requestMap->count();
            _search->searchFromName(requestMap);
        }
    }
}

void SearchTask::checkCountResults()
{
    --_searchResultsCount;
    if(_searchResultsCount <= 0) {
        _dbHelper->dbObject().commit();
        emit finished();
    }
}

