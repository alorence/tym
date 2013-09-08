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

#include "renametask.h"

#include <Logger.h>

#include "commons.h"
#include "dbaccess/bpdatabase.h"

RenameTask::RenameTask(QList<QPair<QFileInfo, QString>> renameMap, QObject *parent) :
    Task(parent),
    _renameMap(renameMap)
{
    _hasMultiResults = true;
}

void RenameTask::run()
{
    BPDatabase db("renameThread");

    emit initializeProgression(_renameMap.size());

    for(QPair<QFileInfo,QString> pair : _renameMap) {
        QFileInfo from(pair.first);
        QString to = from.canonicalPath() + '/' + pair.second;

        QString key = from.filePath();
        emit notifyNewTaskEntity(key, from.fileName());
        emit currentStatusChanged(tr("Renaming file %1").arg(from.fileName()));

        // Progress by steps of 1
        increaseProgressStep();

        // Check if original file exists on disk
        if( ! from.exists()) {
            emit newTaskEntityResult(key, Utils::Error,
                                     tr("%1 does not exists, it "
                                        "can't be renamed.")
                                     .arg(from.canonicalFilePath()));
            continue;
        }

        // Check if target file already exists
        if(QFile::exists(to)) {
            emit newTaskEntityResult(key, Utils::Error,
                                     tr("Target file %1 already exists.")
                                     .arg(to));
            continue;
        }

        // Rename the file
        if( ! QFile(from.canonicalFilePath()).rename(to)) {
            emit newTaskEntityResult(key, Utils::Error,
                                     tr("Error when renaming file %1 into %2")
                                     .arg(from.canonicalFilePath())
                                     .arg(to));
            continue;
        }

        // Check if new file exists on the system
        if( ! QFile::exists(to)) {
            emit newTaskEntityResult(key, Utils::Error,
                                     tr("The file seems to have been renamed, but the "
                                        "new one (%2) can't be found on disk !")
                                     .arg(from.canonicalFilePath())
                                     .arg(pair.second));
            continue;
        }

        emit newTaskEntityResult(key, Utils::Success,
                                 tr("File %1 renamed to %2")
                                 .arg(from.canonicalFilePath())
                                 .arg(QFileInfo(to).fileName()));
        db.renameFile(from.canonicalFilePath(), to);

    }
    emit finished();
}

