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

#ifndef EXPORTPLAYLISTTASK_H
#define EXPORTPLAYLISTTASK_H

#include "tasks/task.h"
#include "wizards/exportplaylistwizard.h"

class BPDatabase;

class ExportPlaylistTask : public Task
{
    Q_OBJECT
public:
    explicit ExportPlaylistTask(const QList<QSqlRecord> &selectedRecords, const QString &filePath, QObject *parent = 0);

    void run() override;

private:

    void writeCollectionEntry(QXmlStreamWriter &xmlDoc, const QSqlRecord &record);
    void writePlaylistEntry(QXmlStreamWriter &xmlDoc, const QSqlRecord &record);

    QFile _outputFile;
    QList<QSqlRecord> _records;
    BPDatabase * _dbHelper;
    QSettings _settings;
};

#endif // EXPORTPLAYLISTTASK_H
