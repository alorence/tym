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
#include <QtGui>
#include <QtSql>

#include <functional>

#include "bpdatabase.h"

class LibraryEntry;

class LibraryModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit LibraryModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QModelIndexList dirNodeModelIndexes() const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    bool hasChildren(const QModelIndex &parent) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QSqlRecord record(const QModelIndex &index);

    QList<QSqlRecord> checkedRecords() const;
    QStringList checkedUids() const;
    int numChecked();

    enum UserRoles {
        UniqueReversePathRole = Qt::UserRole + 1
    };

    enum GroupSelection {
        AllTracks,
        Neither,
        NewTracks,
        MissingTracks,
        LinkedTracks,
        SearchedAndNotLinkedTracks
    };

    void updateSettings();

public slots:
    void refresh();
    void checkSpecificGroup(int checkGroup);

signals:
    void checkedItemsUpdated(int numSelected);
    void rootPathChanged(QString path);

private:
    LibraryEntry* getLibraryNode(const QString &dirPath);
    LibraryEntry* entryFromIndex(const QModelIndex &index) const;

    void recursiveFilteredSetChecked(const QModelIndex &index, const std::function<bool (LibraryEntry *)> &f);
    void setChecked(const QModelIndex &ind, bool checked, bool recursive = true);
    bool isChecked(const QModelIndex &index) const;

    LibraryEntry * _root;

    BPDatabase _db;
    QSqlQuery _elementsList;

    QMap<QString, LibraryEntry*> _dirMap;
    QList<QString> _headers;

    QSet<LibraryEntry*> _checkedEntries;
    static const int CHECKABLECOLUMN;

    bool _colorsEnabled;
    QColor _noResultsColor;
    QColor _missingColor;
    QColor _searchedColor;
    QColor _trackLinkedColor;
};

#endif // LIBRARYMODEL_H
