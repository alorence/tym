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

}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant LibraryModel::data(const QModelIndex &item, int role) const
{
    if (!item.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();


    LibraryEntry *parentDir;
    if(item.parent().isValid()) {
        parentDir = _root;
    } else {
        parentDir = static_cast<LibraryEntry*>(item.parent().internalPointer());
    }

    if(parentDir->isDir(item.row())) {
        LibraryEntry* dir = static_cast<LibraryEntry*>(item.internalPointer());
        if(item.column() == 0) {
            return dir->dir().dirName();
        }
    } else {
        QSqlRecord* record = static_cast<QSqlRecord*>(item.internalPointer());
        return record->value(item.column());
    }

    return QVariant();

}

QModelIndex LibraryModel::index(int row, int column, const QModelIndex &parent) const
{
    if( ! hasIndex(row, column, parent))
        return QModelIndex();

    LibraryEntry *parentDir;
    if(parent.isValid()) {
        parentDir = _root;
    } else {
        parentDir = static_cast<LibraryEntry*>(parent.internalPointer());
    }

    void *childItem = parentDir->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex LibraryModel::parent(const QModelIndex &child) const
{
//    if(child)
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
        while(_elementsList.next()) {
            // Helper to extract some informations from file path
            QFileInfo fileInfo(_elementsList.value(Library::FilePath).toString());

            LibraryEntry* dirEntry = getParentEntry(fileInfo.dir());
            dirEntry->addChild(_elementsList.record());

            LOG_DEBUG(tr("Add child %1 to parent folder %2").arg(_elementsList.value(1).toString()).arg(dirEntry->dir().canonicalPath()));
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
        LOG_DEBUG(tr("Create new Root %1").arg(dir.canonicalPath()));
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
            LibraryEntry* newEntry = new LibraryEntry(dir.canonicalPath(), parentDir);
            _dirMap[dir.canonicalPath()] = newEntry;

            return newEntry;
        }
        // dir is a parent of root, need to update root entry
        else if(_root->dir().canonicalPath().startsWith(dir.canonicalPath())) {

            // Create the future new root element and store it into internal map
            LibraryEntry *newRoot = new LibraryEntry(dir.canonicalPath(), NULL);
            _dirMap[dir.canonicalPath()] = newRoot;


            // Update old root entry
            _root->setParent(getParentEntry(_root->dir().canonicalPath()));

            LOG_DEBUG(tr("Update root %1 -> %2").arg(_root->dir().canonicalPath()).arg(newRoot->dir().canonicalPath()));
            _root = newRoot;
            return newRoot;
        }
        // root and current elements have no common ancestor
        else {
            LibraryEntry *newRoot = new LibraryEntry("/", NULL);
            _dirMap["/"] = newRoot;

            _root->setParent(getParentEntry(_root->dir()));
            LOG_DEBUG(tr("Update root %1 -> %2").arg(_root->dir().canonicalPath()).arg(newRoot->dir().canonicalPath()));
            _root = newRoot;

            LibraryEntry* newEntry = new LibraryEntry(dir.canonicalPath(), getParentEntry(dir));
            _dirMap[dir.canonicalPath()] = newEntry;

            return newEntry;
        }
    }


//    QString rootPath = _root->dir().canonicalPath();
//    QString entryPath = entry->dir().canonicalPath();

//    // root is a child of entry
//    if(rootPath.startsWith(entryPath)) {

//        QString residualPath = rootPath.right(rootPath.size() - entryPath.size());
//        QStringList residualParts = residualPath.split('/', QString::SkipEmptyParts);

//        LibraryEntry *parent = entry;
//        LibraryEntry *child;
//        foreach(QString dirName, residualParts) {
////            child = new LibraryEntry()
//        }

//    }
//    // root is a parent of entry
//    else if(entryPath.startsWith(rootPath)) {

//    } else {
//        QStringList rootDirsList = rootPath.split('/', QString::SkipEmptyParts);
//        QStringList entryDirsList = entryPath.split('/', QString::SkipEmptyParts);

//        // root and entry have a common ancestor
//        if(rootDirsList.first() == entryDirsList.first()) {

//        }
//        // root and entry are stored in a totally different path
//        else {

//        }
//    }
}
