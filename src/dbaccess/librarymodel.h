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

    enum UserRoles {
        UniquePathRole = Qt::UserRole + 1
    };

    enum GroupSelection {
        AllTracks,
        Neither,
        NewTracks,
        MissingTracks,
        LinkedTracks,
        SearchedAndNotLinkedTracks
    };

    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int col, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    /**
     * @brief Return a QSqlRecord corresponding to a given \a index.
     * If index is a dir node, this function returns an empty (invalid) record
     * @param index A index in the model
     * @return Cooresponding sql record
     */
    QSqlRecord record(const QModelIndex &index);

    /**
     * @brief Append records corresponding to given \a index list to the given \a result list.
     * Tracks uids are appended recursively if one of the index given corresponds to a
     * directory node.
     * @param[in] list List of indexes in the model
     * @param[out] result Resulting list of sql records
     */
    void recordsForIndexes(const QModelIndexList &list, QList<QSqlRecord> &result) const;
    /**
     * @brief Append uids corresponding to given \a index list to the given \a result list.
     * Tracks uids are appended recursively if one of the index given corresponds to a
     * directory node.
     * @param[in] list List of indexes in the model
     * @param[out] result Resulting list of uids as strings
     */
    void uidsForIndexes(const QModelIndexList &list, QStringList &result) const;

    /**
     * @brief Append to given \ result indexes which correspond to a track node in the library.
     * If an index to a dir node is given, all its children will be appended. If \a index
     * corresponds to a track in the library, \a result will contains only \a index
     * @param[in] index
     * @param[out] result
     */
    void leafIndexes(const QModelIndex &index, QSet<QModelIndex> &result) const;

    /**
     * @brief Return all indexes corresponding to directory nodes in the model.
     * @return List of dir nodes as a list of QModelIndex
     */
    QModelIndexList dirNodeModelIndexes() const;

    const QModelIndexList indexesForGroup(GroupSelection group) const;

    /**
     * @brief Update library configuration according to settings registered by user.
     * For the moment, this only affects colors of elements in the library.
     */
    void updateSettings();

public slots:
    void refresh();

signals:
    /**
     * @brief Signal emitted when a refresh modify the root folder containing all tracks
     * in the library.
     * @param path
     */
    void rootPathChanged(QString path);

private:
    LibraryEntry* getLibraryNode(const QString &dirPath);
    LibraryEntry* entryFromIndex(const QModelIndex &index) const;

    void filteredIndexes(const QModelIndex &index,
                         const std::function<bool (const LibraryEntry *)> &filterFunction,
                         QModelIndexList &result) const;

    LibraryEntry * _root;

    BPDatabase _db;
    QSqlQuery _elementsList;

    QMap<QString, LibraryEntry*> _dirMap;
    QList<QString> _headers;

    bool _colorsEnabled;
    QColor _noResultsColor;
    QColor _missingColor;
    QColor _searchedColor;
    QColor _trackLinkedColor;
};

#endif // LIBRARYMODEL_H
