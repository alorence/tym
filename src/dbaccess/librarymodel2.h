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

#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QtCore>
#include <QtSql>

#include "bpdatabase.h"

class LibraryEntry;

class LibraryModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit LibraryModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    /* TODO : to replace this method to use a modelindex */
    QSqlRecord record(int i);

    QSet<int> selectedIds() const;
    QHash<int, QSqlRecord> selectedRecords() const;

public slots:
    void refresh();
    void unselectRowsAndRefresh(QList<int> rows);


private:
    LibraryEntry* getParentEntry(const QDir &file);
    LibraryEntry * _root;

    BPDatabase _db;
    QSqlQuery _elementsList;

    QMap<QString, LibraryEntry*> _dirMap;
};

#endif // LIBRARYMODEL_H
