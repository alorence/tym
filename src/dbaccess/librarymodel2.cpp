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

#include <Logger.h>
#include "commons.h"
#include "libraryentry.h"

LibraryModel::LibraryModel(QObject *parent) :
    QAbstractItemModel(parent),
    _db("libraryModel", this),
    _elementsList("SELECT * FROM LibraryHelper ORDER BY filePath", _db.dbObject()),
    _root(NULL)
{
    _headers << tr("Name") << tr("Status") << tr("Results") << tr("Infos");
    refresh();
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant LibraryModel::data(const QModelIndex &item, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    LibraryEntry* entry = entryFromIndex(item);

    if(entry->isDirNode() && item.column() == 0) {
        return entry->dir().dirName();
    } else if(! entry->isDirNode()) {
        QVariant tpoto = entry->data((LibraryEntry::DataIndexes) item.column());
        qDebug() << tpoto;
        return tpoto;
    } else {
        return QVariant();
    }
}

QModelIndex LibraryModel::index(int row, int column, const QModelIndex &parent) const
{
    if( ! hasIndex(row, column, parent))
        return QModelIndex();

    LibraryEntry *parentEntry = entryFromIndex(parent);

    LibraryEntry *childItem = parentEntry->child(row);

    if (childItem != NULL){
        return createIndex(row, column, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex LibraryModel::parent(const QModelIndex &child) const
{
    LibraryEntry* parent = entryFromIndex(child)->parent();

    if(parent == _root) {
        return QModelIndex();
    } else {
        return createIndex(parent->rowPosition(), 0, parent);
    }
}

int LibraryModel::rowCount(const QModelIndex &parent) const
{
    if(entryFromIndex(parent) == _root) {
        qDebug() << "root rows:" << entryFromIndex(parent)->rowCount();
    }
    return entryFromIndex(parent)->rowCount();
}

int LibraryModel::columnCount(const QModelIndex &parent) const
{
    return 4;
}

QVariant LibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _headers[section];

    return QVariant();
}

QSqlRecord LibraryModel::record(int i)
{
    return QSqlRecord();
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
    if( ! _elementsList.exec()) {
        LOG_ERROR(tr("Unable to refresh library model : %1").arg(_elementsList.lastError().text()));
    } else {
        delete _root;
        _root = NULL;

        while(_elementsList.next()) {
            // Helper to extract some informations from file path
            QFileInfo fileInfo(_elementsList.value(Library::FilePath).toString());

            LibraryEntry* parentDir = getParentEntry(fileInfo.dir());
            new LibraryEntry(_elementsList.record(), parentDir);

            LOG_DEBUG(tr("Add child %1 to parent folder %2").arg(_elementsList.value(1).toString()).arg(parentDir->dir().canonicalPath()));
            qDebug() << _dirMap;
        }
    }

    LOG_DEBUG(tr("Root is %1").arg(_root->dir().canonicalPath()));
}

void LibraryModel::unselectRowsAndRefresh(QList<int> rows)
{
}

LibraryEntry *LibraryModel::getParentEntry(const QDir &dir)
{
    if(_root == NULL) {
        _root = new LibraryEntry(dir.canonicalPath(), NULL);
        _dirMap[dir.canonicalPath()] = _root;
//        LOG_DEBUG(tr("Create new Root %1").arg(dir.canonicalPath()));
        return _root;
    }

    // Parent directory already exist
    if(_dirMap.contains(dir.canonicalPath())) {
        return _dirMap.value(dir.canonicalPath());
    } else {

        // dir is a subentry of root
        if(dir.canonicalPath().startsWith(_root->dir().canonicalPath())) {
            QDir upDir = dir;
            LibraryEntry* parentDir;
            if(upDir.cdUp()) {
                parentDir = getParentEntry(upDir);
            } else {
                parentDir = _root;
            }
//            LOG_DEBUG(tr("Create new dir entry : %1").arg(dir.canonicalPath()));
            LibraryEntry* newEntry = new LibraryEntry(dir, parentDir);
            _dirMap[dir.canonicalPath()] = newEntry;

            return newEntry;
        }
        // dir is a parent of root, need to update root entry
        else if(_root->dir().canonicalPath().startsWith(dir.canonicalPath())) {

            // Create the future new root element and store it into internal map
            LibraryEntry *newRoot = new LibraryEntry(dir, NULL);
            _dirMap[dir.canonicalPath()] = newRoot;
            LibraryEntry * oldRoot = _root;
//            LOG_DEBUG(tr("Update root1 %1 -> %2").arg(oldRoot->dir().canonicalPath()).arg(newRoot->dir().canonicalPath()));
            _root = newRoot;

            // Update old root entry
            LibraryEntry * newRootParent = getParentEntry(_root->dir().canonicalPath());
//            LOG_DEBUG(tr("Update old root parent : %1").arg(newRootParent->dir().canonicalPath()));
            oldRoot->setParent(newRootParent);

            return newRoot;
        }
        // root and current elements have no common ancestor
        else {
            LibraryEntry *newRoot = new LibraryEntry(QDir("/"), NULL);
            _dirMap["/"] = newRoot;

            _root->setParent(getParentEntry(_root->dir()));
//            LOG_DEBUG(tr("Update root2 %1 -> %2").arg(_root->dir().canonicalPath()).arg(newRoot->dir().canonicalPath()));
            _root = newRoot;

            LibraryEntry* newEntry = new LibraryEntry(dir.canonicalPath(), getParentEntry(dir));
            _dirMap[dir.canonicalPath()] = newEntry;

            return newEntry;
        }
    }
}

QString LibraryModel::debug(const QModelIndex &index) const
{
    LibraryEntry* p = static_cast<LibraryEntry*>(index.internalPointer());

    QString name;
    if(p == NULL) {
        name = "NULL";
    } else if(p->isDirNode()) {
        name = p->dir().dirName();
    } else {
        name = p->data(LibraryEntry::Name).toString();
    }

    return QString("[%1, %2, %3]").arg(index.row()).arg(index.column()).arg(name);
}

LibraryEntry *LibraryModel::entryFromIndex(const QModelIndex &index) const
{
    if( index.isValid()) {
        return static_cast<LibraryEntry*>(index.internalPointer());
    } else {
        return _root;
    }
}
