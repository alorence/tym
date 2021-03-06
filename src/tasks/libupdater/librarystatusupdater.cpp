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

#include "librarystatusupdater.h"

#include "dbaccess/bpdatabase.h"
#include "commons.h"

LibraryStatusUpdater::LibraryStatusUpdater(QObject *parent) :
    Task(parent),
    _dbHelper(new BPDatabase("libraryStatusUpdater", this))
{
}

LibraryStatusUpdater::~LibraryStatusUpdater()
{
    delete _dbHelper;
}

void LibraryStatusUpdater::run()
{
    QList<QSqlRecord> missingList;

    QSqlQuery query = _dbHelper->libraryInformations();
    while(query.next()) {
        if( ((Library::FileStatus) query.value(Library::Status).toInt()).testFlag(Library::FileNotFound) ) {
            continue;
        }

        QFileInfo fileToCheck(query.value(Library::FilePath).toString());
        if( ! fileToCheck.exists()) {
            missingList << query.record();
            LOG_INFO(QString("Track %1 missing, status will be updated.").arg(fileToCheck.absoluteFilePath()));
        }
    }

    query.finish();

    for(QSqlRecord missing : missingList) {
        _dbHelper->updateLibraryStatus(missing.value(Library::Uid).toString(),
                                       Library::FileNotFound | (Library::FileStatus) missing.value(Library::Status).toInt());
    }

    emit finished("All tracks have been updated");
}
