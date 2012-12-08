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

#include "librarymodel.h"
#include "searchresultsmodel.h"
#include "src/commons.h"



class BPDatabase : public QObject
{
    Q_OBJECT

public:
    static BPDatabase * instance();
    static void deleteInstance();

    bool initialized();
    bool initDB();
    QString version();
    static QSqlDatabase dbObject();

    LibraryModel * libraryModel() const;
    SearchResultsModel * searchModel() const;

    QSqlRecord trackInformations(QVariant & bpid);
    void deleteFromLibrary(QVariantList &uids);

public slots:
    void storeSearchResults(QString libId, QVariant result);
    QVariant storeTrack(const QVariant track);
    void importFile(QString filePath);
    void importFiles(const QStringList &);
    bool setLibraryTrackReference(QString libUid, QVariant bpid);
    void updateSearchResults(const QModelIndex&,const QModelIndex&);
    void updateLibraryStatus(QString uid, FileStatus::Status status);

private :
    explicit BPDatabase(QObject *parent = 0);
    BPDatabase(const BPDatabase &);
    BPDatabase operator=(const BPDatabase &);
    ~BPDatabase();
    static BPDatabase * _instance;

    LibraryModel *_libraryModel;
    SearchResultsModel * _searchModel;

    bool initTables();
    bool dbInitialized;

    QSqlRecord basicLibraryRecord;
};

#endif // BPDATABASE_H
