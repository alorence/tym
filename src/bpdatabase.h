/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag your Music).
*
* TYM (Tag your Library) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag your Library) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag your Library).  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BPDATABASE_H
#define BPDATABASE_H

#include <QtCore>
#include <QtSql>

#include "src/librarymodel.h"
#include "commons.h"

class BPDatabase : public QObject
{
    Q_OBJECT

public:
    explicit BPDatabase(QObject *parent = 0);

    bool databaseConnect();
    QString version();

    static const QSqlDatabase dbObject();

    LibraryModel * libraryModel() const;
    QSqlQueryModel * searchModel() const;

public slots:
    void storeSearchResults(int row, QVariant result);
    QVariant storeTrack(const QVariant track);
    void importFile(QString filePath);
    void importFiles(const QStringList &);
    bool setLibraryTrackReference(int row, QVariant bpid);
    void updateSearchResults(const QModelIndex&,const QModelIndex&);
    void updateLibraryStatus(int uid, FileStatus::Status status);

private :
    LibraryModel *_libraryModel;
    void initTables();

    QSqlQueryModel * _searchModel;
    QSqlQuery _searchQuery;

    QSqlRecord basicLibraryRecord;
};

#endif // BPDATABASE_H
