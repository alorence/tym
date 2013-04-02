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

#include "librarymodel.h"

#include <QtWidgets>

#include <Logger.h>
#include "commons.h"
#include "libraryentry.h"
#include "tools/utils.h"

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
    if(index.column() == 0) {
        Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
        return  defaultFlags | Qt::ItemIsUserCheckable;
    }
    return QAbstractItemModel::flags(index);
}

QVariant LibraryModel::data(const QModelIndex &item, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::DecorationRole && role != Qt::CheckStateRole)
        return QVariant();

    LibraryEntry* entry = entryFromIndex(item);

    if(role == Qt::DisplayRole) {
        if(entry->isDirNode() && item.column() == 0) {
            return entry->dir().dirName();
        } else if(! entry->isDirNode()) {
            return entry->data((Library::GuiIndexes) item.column());
        } else {
            return QVariant();
        }
    } else if(role == Qt::DecorationRole && item.column() == 0) {
        QString iconType = entry->isDirNode() ? "folder" : "file";
        return QPixmap (":/img/icons/general/" + iconType);
    } else if(role == Qt::CheckStateRole && item.column() == 0) {
        return isChecked(item) ? Qt::Checked : Qt::Unchecked;
    }
    return QVariant();
}

bool LibraryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role != Qt::CheckStateRole) {
        return false;
    }

    setChecked(index, value == Qt::Checked);
    return true;
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
    return entryFromIndex(parent)->rowCount();
}

int LibraryModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant LibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return _headers[section];

    return QVariant();
}

QSqlRecord LibraryModel::record(const QModelIndex &index)
{
    LibraryEntry * item = entryFromIndex(index);
    if(item->isDirNode()) {
        return QSqlRecord();
    } else {
        return item->record();
    }
}

QList<QSqlRecord> LibraryModel::checkedRecords() const
{
    QList<QSqlRecord> result;
    foreach (LibraryEntry* entry, _checkedEntries) {
        if(!entry->isDirNode()) {
            result << entry->record();
        }
    }
    return result;
}

QStringList LibraryModel::checkedUids() const
{
    QStringList result;
    foreach (LibraryEntry* entry, _checkedEntries) {
        if(!entry->isDirNode()) {
            result << entry->data(Library::Name).toString();
        }
    }
    return result;
}

int LibraryModel::numChecked()
{
    return _checkedEntries.size();
}

void LibraryModel::refresh()
{
    if( ! _elementsList.exec()) {
        LOG_ERROR(tr("Unable to refresh library model : %1").arg(_elementsList.lastError().text()));
    } else {
        // Invalidate all existing ModelIndexes
        beginResetModel();

        // All root's children are deleted and cleaned
        delete _root;
        _root = NULL;

        // Reset the directories map and checked entries
        _dirMap.clear();
        _checkedEntries.clear();;

        while(_elementsList.next()) {
            // Helper to extract some informations from file path
            QFileInfo fileInfo(_elementsList.value(Library::FilePath).toString());

            LibraryEntry* parentDir = getLibraryNode(fileInfo.dir());
            new LibraryEntry(_elementsList.record(), parentDir);

            LOG_DEBUG(tr("Add child %1 to parent folder %2").arg(_elementsList.value(1).toString()).arg(parentDir->dir().canonicalPath()));
            qDebug() << _dirMap << _root->dir().canonicalPath();
        }

        // Inform view that new items are stored in the model. It will start te rebuild its ModelIndexes
        endResetModel();
    }

    LOG_DEBUG(tr("Root is %1").arg(_root->dir().canonicalPath()));
}

void LibraryModel::unselectRowsAndRefresh(QList<int> rows)
{
}

LibraryEntry *LibraryModel::getLibraryNode(const QDir &dir)
{
    QString dirUnifiedPath = Utils::unifyPath(dir.canonicalPath());

    if(_root == NULL) {
        _root = new LibraryEntry(dir.canonicalPath(), NULL);
        _dirMap[dirUnifiedPath] = _root;
        //LOG_DEBUG(tr("Create new Root %1").arg(dir.canonicalPath()));
        return _root;
    }

    QString rootUnifiedPath = Utils::unifyPath(_root->dir().canonicalPath());

    // Parent directory already exist
    if(_dirMap.contains(dirUnifiedPath)) {
        return _dirMap.value(dirUnifiedPath);
    } else {
        // dir is a subentry of root
        if(dirUnifiedPath.startsWith(rootUnifiedPath)) {
            QDir upDir = dir;
            LibraryEntry* parentDir;
            if(upDir.cdUp()) {
                parentDir = getLibraryNode(upDir);
            } else {
                // FIXME: should never come here...
                parentDir = _root;
            }
            LOG_DEBUG(tr("**1** Create new dir entry : %1").arg(dir.canonicalPath()));
            LibraryEntry* newEntry = new LibraryEntry(dir, parentDir);
            _dirMap[dirUnifiedPath] = newEntry;

            return newEntry;
        }
        // dir is a parent of root, need to update root entry
        else if(rootUnifiedPath.startsWith(dirUnifiedPath)) {

            // Stroe the current root, before updating it
            LibraryEntry *oldRoot = _root;

            // Create the future new root element and update current root & internal map
            _dirMap[dirUnifiedPath] = _root = new LibraryEntry(dir, NULL);

            LOG_DEBUG(tr("**2** Update root1 %1 -> %2").arg(oldRoot->dir().canonicalPath()).arg(_root->dir().canonicalPath()));

            QDir oldRootDir = oldRoot->dir();
            oldRootDir.cdUp();

            // Update old root parent member
            oldRoot->setParent(getLibraryNode(oldRootDir));
            //LOG_DEBUG(tr("Update old root parent : %1").arg(newRootParent->dir().canonicalPath()));

            return _root;
        }
        // root and current elements have no common ancestor
        else {

            QDir upDir = dir;
            upDir.cdUp();

            LOG_DEBUG(tr("**3** Check up dir"));

            LibraryEntry* newEntry = new LibraryEntry(dir.canonicalPath(), getLibraryNode(upDir));
            _dirMap[dirUnifiedPath] = newEntry;

            /*
            LibraryEntry *oldRoot = _root;

            LibraryEntry *newRoot = new LibraryEntry(QDir("/"), NULL);
            _dirMap["/"] = newRoot;
            _root = newRoot;

            oldRoot->setParent(getParentDirEntry(oldRoot->dir()));
            //LOG_DEBUG(tr("Update root2 %1 -> %2").arg(rootUnifiedPath).arg(newRoot->dir().canonicalPath()));


            LibraryEntry* newEntry = new LibraryEntry(dir.canonicalPath(), getParentDirEntry(dir));
            _dirMap[dir.canonicalPath()] = newEntry;
            */

            return newEntry;
        }
    }
}

LibraryEntry *LibraryModel::entryFromIndex(const QModelIndex &index) const
{
    if(index.isValid()) {
        return static_cast<LibraryEntry*>(index.internalPointer());
    } else {
        return _root;
    }
}

void LibraryModel::setChecked(const QModelIndex &ind, bool checked)
{
    LibraryEntry *entry = entryFromIndex(ind);
    // Check rows not already checked
    if(checked && !_checkedEntries.contains(entry)) {
        // Check the entry
        _checkedEntries.insert(entry);
        emit dataChanged(ind, ind, QVector<int>() << Qt::CheckStateRole);

        // Check its parent if all sibling are checked
        bool allChecked = true;
        foreach(LibraryEntry* sibling, entry->parent()->children()) {
            if(!_checkedEntries.contains(sibling)) {
                allChecked = false;
                break;
            }
        }
        if(allChecked) {
            _checkedEntries.insert(entry->parent());
            emit dataChanged(ind.parent(), ind.parent(),
                             QVector<int>() << Qt::CheckStateRole);
        }

        // If entry is a dir, check all its children
        if(entry->isDirNode()) {
            foreach(LibraryEntry* child, entry->children()) {
                _checkedEntries.insert(child);
            }

            emit dataChanged(ind.child(0, 0), ind.child(rowCount(ind) - 1, 0),
                             QVector<int>() << Qt::CheckStateRole);

        }
    }
    // Uncheck rows already checked
    else if (!checked && _checkedEntries.contains(entry)) {
        // Uncheck the entry
        _checkedEntries.remove(entry);
        emit dataChanged(ind, ind, QVector<int>() << Qt::CheckStateRole);

        // Uncheck its parent if it was checked
        if(_checkedEntries.contains(entry->parent())){
            _checkedEntries.remove(entry->parent());
            emit dataChanged(ind.parent(), ind.parent(),
                             QVector<int>() << Qt::CheckStateRole);
        }

        // Uncheck all its children if it is a dir
        if(entry->isDirNode()) {
            foreach(LibraryEntry* child, entry->children()) {
                _checkedEntries.remove(child);
            }

            emit dataChanged(ind.child(0, 0), ind.child(rowCount(ind) - 1, 0),
                             QVector<int>() << Qt::CheckStateRole);
        }
    }
    emit checkedItemsUpdated(_checkedEntries.size());
}

bool LibraryModel::isChecked(const QModelIndex &index) const
{
    return _checkedEntries.contains(entryFromIndex(index));
}
