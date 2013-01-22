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

#include "commons.h"

class LibraryModel;
class SearchResultsModel;

class BPDatabase : public QObject
{
    Q_OBJECT

public:
    explicit BPDatabase(QString connectionName = "defaultConnection", QObject *parent = 0);
    ~BPDatabase();

    bool initialized();
    QString version();
    QSqlDatabase dbObject();

    QSqlRecord trackInformations(QString &bpid);
    QSqlQuery tracksInformations(QStringList &bpids);
    void deleteFromLibrary(QStringList uids);
    void deleteSearchResult(QString libId, QString trackId);

    void renameFile(QString &oldFileName, QString &newFileName);

    friend class LibraryModel;

public slots:
    void storeSearchResults(QString libId, QJsonValue result);
    void importFile(QString filePath);
    void importFiles(const QStringList &);
    void updateLibraryStatus(QString uid, Library::FileStatus status);
    bool setLibraryTrackReference(QString libUid, QString bpid);

signals:
    void libraryEntryUpdated(QString uid);
    void referenceForTrackUpdated(QString uid);
    void searchResultStored(QString);

private :
    QSqlDatabase _dbObject;
    QString storeTrack(const QJsonValue track);

    bool initTables();
    bool _dbInitialized;

    QMutex *_dbMutex;
};

#endif // BPDATABASE_H
