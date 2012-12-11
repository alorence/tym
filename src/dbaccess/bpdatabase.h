/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag Your Music) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag Your Music) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag Your Music).  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BPDATABASE_H
#define BPDATABASE_H

#include <QtCore>
#include <QtSql>

#include "src/commons.h"

class LibraryModel;
class SearchResultsModel;

class BPDatabase : public QObject
{
    Q_OBJECT

public:
    static BPDatabase * instance();
    static void deleteInstance();

    static QString const MAIN_DB;
    static QString const THREAD_DB;

    bool initialized();
    bool initDB();
    QString version();
    static QSqlDatabase dbObject(const QString &dbId = THREAD_DB);

    QSqlRecord trackInformations(QVariant & bpid);
    void deleteFromLibrary(QStringList uids);
    void deleteSearchResult(QString libId, QString trackId);

    friend class LibraryModel;

public slots:
    void storeSearchResults(QString libId, QVariant result);
    QString storeTrack(const QVariant track);
    void importFile(QString filePath);
    void importFiles(const QStringList &);
    bool setLibraryTrackReference(QString libUid, QString bpid);
    void updateLibraryStatus(QString uid, FileStatus::Status status);

signals:
    void libraryEntryUpdated(QString uid);
    void referenceForTrackUpdated(QString uid);

private :
    explicit BPDatabase(QObject *parent = 0);
    BPDatabase(const BPDatabase &);
    BPDatabase operator=(const BPDatabase &);
    ~BPDatabase();
    static BPDatabase * _instance;

    bool initTables();
    bool dbInitialized;

    QMutex *dbMutex;

    QSqlRecord basicLibraryRecord;
};

#endif // BPDATABASE_H
