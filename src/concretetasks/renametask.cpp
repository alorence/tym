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

#include "renametask.h"

#include <Logger.h>

#include "commons.h"
#include "dbaccess/bpdatabase.h"

RenameTask::RenameTask(QHash<QString, QString> renameMap, QObject *parent) :
    Task(parent)
{
    _renameMap = renameMap;
}

void RenameTask::run()
{
    BPDatabase db("renameThread");
    QHashIterator<QString, QString> it(_renameMap);

    QRegularExpression renameValidityCheck("^<.*>$");

    while(it.hasNext()) {
        QFileInfo from(it.next().key());
        QString to = it.value();

        // Rename only if the target filename is not "<...>"
        if( ! renameValidityCheck.match(to).hasMatch()) {
            if( ! from.exists()) {
                LOG_WARNING(tr("Error, file %1 does not exists, it can't be renamed.")
                            .arg(from.canonicalFilePath()));
                continue;
            }

            if(QFile::exists(to)) {
                LOG_WARNING(tr("Error when renaming file %1, file %2 already exists.")
                            .arg(from.canonicalFilePath())
                            .arg(to));
                continue;
            }

            if(QFile::rename(from.canonicalFilePath(), to)) {
                LOG_INFO(tr("File %1 renamed to %2")
                         .arg(from.canonicalFilePath())
                         .arg(QFileInfo(to).fileName()));
            } else {
                LOG_WARNING(tr("Error when renaming file %1 into %2")
                            .arg(from.canonicalFilePath())
                            .arg(to));
                continue;
            }

            // TODO : check references rules to understand why GCC can't automatically get reference
            QString cannonPath(from.canonicalFilePath());
            db.renameFile(cannonPath, to);
        }

        emit finished();
    }
}

