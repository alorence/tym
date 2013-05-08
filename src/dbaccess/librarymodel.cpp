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
    if (role != Qt::DisplayRole && role != Qt::DecorationRole && role != Qt::CheckStateRole
            && !(role == UniqueReversePathRole && entryFromIndex(item)->isDirNode()))
        return QVariant();

    LibraryEntry* entry = entryFromIndex(item);

    if(role == Qt::DisplayRole) {
        if(entry->isDirNode() && item.column() == 0) {
            return entry->dirName();
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
    } else if (role == UniqueReversePathRole) {
        QString result;
        LibraryEntry *entry = entryFromIndex(item);
        do {
            result.append(entry->dirName());
            entry = entry->parent();
        } while (entry != NULL);
        return result;
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

QModelIndexList LibraryModel::dirNodeModelIndexes() const
{
    QModelIndexList dirNodeList;
    for(QModelIndex ind : persistentIndexList()) {
        if(entryFromIndex(ind)->isDirNode()) {
            dirNodeList << ind;
        }
    }
    return dirNodeList;
}

QModelIndex LibraryModel::index(int row, int column, const QModelIndex &parent) const
{
    if(! hasIndex(row, column, parent))
        return QModelIndex();

    LibraryEntry *childItem = entryFromIndex(parent)->child(row);

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
    LibraryEntry* entry = entryFromIndex(parent);
    if(entry == NULL) {
        return 0;
    } else {
        return entry->rowCount();
    }
}

int LibraryModel::columnCount(const QModelIndex &) const
{
    return 4;
}

bool LibraryModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid() || entryFromIndex(parent)->isDirNode();
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
            result << entry->record().value(Library::Uid).toString();
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

        // All items are deleted
        delete _dirMap["/"];

        // Reset the directories map and checked entries
        _dirMap.clear();
        _checkedEntries.clear();;

        _dirMap["/"] = new LibraryEntry();;
        _root = _dirMap["/"];

        while(_elementsList.next()) {
            // Helper to extract some informations from file path
            QFileInfo fileInfo(_elementsList.value(Library::FilePath).toString());

            LibraryEntry* parentDir = getLibraryNode(fileInfo.canonicalPath());
            new LibraryEntry(_elementsList.record(), parentDir);
        }

        // Inform view that new items are stored in the model. It will start te rebuild its ModelIndexes
        endResetModel();
    }
}

LibraryEntry *LibraryModel::getLibraryNode(const QString &dirPath)
{
    if(_dirMap.contains(dirPath)) {
        return _dirMap[dirPath];
    }

    // We need to create a new dir entry
    LibraryEntry *currentDirEntry;
    QDir currentDir(dirPath);
    if(currentDir.isRoot()) {
        currentDirEntry = new LibraryEntry(currentDir.path().remove(QRegularExpression("/$")), _dirMap["/"]);
    } else {
        QDir upDir(currentDir);
        upDir.cdUp();
        LibraryEntry *parentEntry = getLibraryNode(upDir.canonicalPath());

        currentDirEntry = new LibraryEntry(currentDir.dirName(), parentEntry);
    }

    // Update _root pointer
    LibraryEntry *r = _dirMap["/"];
#ifdef Q_OS_WIN
    QString path = tr("Computer");
#else
    QString path = "/";
#endif
    while(r->rowCount() == 1) {
        r = r->child(0);
        path.append(QDir::separator()).append(r->dirName());
    }
    if(r != _root) {
        _root = r;
        emit rootPathChanged(path);
    }

    _dirMap[dirPath] = currentDirEntry;
    return currentDirEntry;
}

LibraryEntry *LibraryModel::entryFromIndex(const QModelIndex &index) const
{
    if(index.isValid()) {
        return static_cast<LibraryEntry*>(index.internalPointer());
    } else {
        return _root;
    }
}

void LibraryModel::setChecked(const QModelIndex &ind, bool checked, bool recursive)
{
    LibraryEntry *entry = entryFromIndex(ind);
    // Check rows not already checked
    if(checked && !_checkedEntries.contains(entry)) {
        // Check the entry
        _checkedEntries.insert(entry);
        emit dataChanged(ind, ind, QVector<int>() << Qt::CheckStateRole);

        // Check entry's parent if it is not root and all sibling are checked
        if(ind.parent().isValid()) {
            bool allChecked = true;
            foreach(LibraryEntry* sibling, entry->parent()->children()) {
                if(!_checkedEntries.contains(sibling)) {
                    allChecked = false;
                    break;
                }
            }
            if(allChecked) {
                setChecked(ind.parent(), true, false);
            }
        }

        // If entry is a dir, check all its children
        if(recursive && entry->isDirNode()) {
            for(int i = 0 ; i < rowCount(ind) ; ++i) {
                setChecked(index(i, ind.column(), ind), true, true);
            }
        }
    }
    // Uncheck rows already checked
    else if (!checked && _checkedEntries.contains(entry)) {
        // Uncheck the entry
        _checkedEntries.remove(entry);
        emit dataChanged(ind, ind, QVector<int>() << Qt::CheckStateRole);

        // Uncheck its parent if it was checked
        if(_checkedEntries.contains(entry->parent())){
            setChecked(ind.parent(), false, false);
        }

        // Uncheck all its children if it is a dir
        if(recursive && entry->isDirNode()) {
            for(int i = 0 ; i < rowCount(ind) ; ++i) {
                setChecked(index(i, ind.column(), ind), false, true);
            }
        }
    }
    emit checkedItemsUpdated(_checkedEntries.size());
}

bool LibraryModel::isChecked(const QModelIndex &index) const
{
    return _checkedEntries.contains(entryFromIndex(index));
}
