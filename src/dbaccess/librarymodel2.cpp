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

#include "librarymodel2.h"

#include <QtWidgets>

#include "commons.h"
#include "bpdatabase.h"
#include "libraryentry.h"

LibraryModel::LibraryModel(QObject *parent) :
    QAbstractItemModel(parent)
{

}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant LibraryModel::data(const QModelIndex &item, int role) const
{
    return QVariant();
}

QModelIndex LibraryModel::index(int row, int column, const QModelIndex &parent) const
{
    return QModelIndex();
}

QModelIndex LibraryModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int LibraryModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int LibraryModel::columnCount(const QModelIndex &parent) const
{
    return 0;
}

QSqlRecord LibraryModel::record(int i)
{
    return QSqlRecord    ();
}

QSet<int> LibraryModel::selectedIds() const
{
    return QSet<int>();
}

QHash<int, QSqlRecord> LibraryModel::selectedRecords() const
{
    return QHash<int, QSqlRecord>();
}


void LibraryModel::refresh()
{

}

void LibraryModel::unselectRowsAndRefresh(QList<int> rows)
{
}
