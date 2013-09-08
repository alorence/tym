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

#include <QSqlRecord>
#include <QXmlStreamWriter>
#include <QFile>
#include <QSettings>
#include "tasks/task.h"

class BPDatabase;

class ExportTask : public Task
{
    Q_OBJECT
public:
    explicit ExportTask(const QList<QSqlRecord> &selectedRecords, const QString &filePath,
                                bool exportPlaylist = false, QObject *parent = 0);
    /**
     * @brief Reimplementation of Task::run()
     */
    void run() override;

private:
    /**
     * @brief Write in the \i xmlDoc stream the collection tag corresponding to the given \i record.
     * @param xmlDoc
     * @param record
     */
    void writeCollectionEntry(QXmlStreamWriter &xmlDoc, const QSqlRecord &record);
    /**
     * @brief Write in the \i xmlDoc stream the playlist tag corresponding to the given \i record.
     * @param xmlDoc
     * @param record
     */
    void writePlaylistEntry(QXmlStreamWriter &xmlDoc, const QSqlRecord &record);

    QFile _outputFile;
    QList<QSqlRecord> _records;
    BPDatabase * _dbHelper;
    QSettings _settings;
    bool _exportPlaylist;
};

#endif // EXPORTPLAYLISTTASK_H
