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

    const bool initialized() const;
    const QString version() const;
    QSqlDatabase dbObject() const;

    const QSqlRecord trackInformations(const QString &bpid) const;
    const QSqlQuery tracksInformations(const QStringList &bpids = QStringList());
    const QSqlQuery libraryInformations(const QStringList &uids = QStringList());
    void deleteLibraryEntry(QStringList uids) const;
    void deleteSearchResult(const QString &libId, const QString &trackId) const;

    void renameFile(const QString &oldFileName, const QString &newFileName) const;

    friend class LibraryModel;

public slots:
    void storeSearchResults(const QString &libId, const QJsonValue &result) const;
    void importFile(const QString &filePath) const;
    void importFiles(const QStringList &) const;
    void updateLibraryStatus(const QString &uid, const Library::FileStatus &status) const;
    const bool setLibraryTrackReference(const QString &libUid, const QString &bpid) const;

signals:
    void libraryEntryUpdated(const QString &uid = QString()) const;
    void referenceForTrackUpdated(const QString &uid) const;
    void searchResultStored(const QString& uid) const;

private :
    QSqlDatabase _dbObject;
    const QString storeTrack(const QJsonValue &track) const;

    const bool initTables() const;
    bool _dbInitialized;

    QMutex *_dbMutex;
};

#endif // BPDATABASE_H
