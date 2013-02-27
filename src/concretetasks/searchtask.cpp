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

SearchTask::SearchTask(QString searchPattern, QList<QSqlRecord> selectedRecords, QObject *parent) :
    Task(parent),
    _searchPattern(searchPattern),
    _selectedRecords(selectedRecords),
    _dbHelper(new BPDatabase("searchTask", this)),
    _search(new SearchProvider(this)),
    _searchResultsCount(0)
{

    connect(_search, &SearchProvider::searchResultAvailable, _dbHelper, &BPDatabase::storeSearchResults);

    connect( _dbHelper, &BPDatabase::searchResultStored, this, &SearchTask::checkCountResults);
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


}

void SearchTask::checkCountResults()
{
    --_searchResultsCount;
    if(_searchResultsCount <= 0) {
        _dbHelper->dbObject().commit();
        emit finished();
    }
}

