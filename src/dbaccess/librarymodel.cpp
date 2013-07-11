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
    _root(nullptr),
    _db("libraryModel", this),
    _elementsList(_db.dbObject())

{
    _headers << tr("Name") << tr("Status") << tr("Results") << tr("Infos");

    _noResultsColor = QColor(Qt::gray);
    _noResultsColor.setAlpha(30);
    _missingColor = QColor(Qt::red);
    _missingColor.setAlpha(60);
    _searchedColor = QColor(254, 183, 11, 50);
    _trackLinkedColor = QColor(Qt::green);
    _trackLinkedColor.setAlpha(50);

    _elementsList.prepare("SELECT * FROM LibraryHelper ORDER BY filePath");

    updateSettings();
}

QVariant LibraryModel::data(const QModelIndex &item, int role) const
{
    QSet<Qt::ItemDataRole> enabledRoles;
    enabledRoles << Qt::DisplayRole << Qt::DecorationRole;

    if(_colorsEnabled) {
        enabledRoles << Qt::BackgroundColorRole;
    }

    if ( ! enabledRoles.contains((Qt::ItemDataRole)role)
         && !(role == UniquePathRole && entryFromIndex(item)->isDirNode()))
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
        return QPixmap (":/generic_icons/" + iconType);
    } else if (role == UniquePathRole) {

        return _dirMap.key(entryFromIndex(item), "/");
    } else if (role == Qt::BackgroundColorRole) {
        // Set BG color for each row
        Library::FileStatus status =
                (Library::FileStatus) entry->record().value(Library::Status).toInt();

        if(status.testFlag(Library::FileNotFound)) {
            return QBrush(_missingColor);
        } else if(status.testFlag(Library::Searched)) {
            bool hasLink = !entry->record().value(Library::Bpid).isNull();
            int numResults = entry->record().value(Library::NumResults).toInt();
            if(hasLink) {
                return QBrush(_trackLinkedColor);
            } else if(numResults) {
                return QBrush(_searchedColor);
            } else {
                return QBrush(_noResultsColor);
            }
        }
        return QVariant();
    }
    return QVariant();
}

QModelIndex LibraryModel::index(int row, int col, const QModelIndex &parent) const
{
    if(! hasIndex(row, col, parent))
        return QModelIndex();

    LibraryEntry *childItem = entryFromIndex(parent)->child(row);

    if (childItem != nullptr){
        return createIndex(row, col, childItem);
    } else {
        return QModelIndex();
    }
}

QModelIndex LibraryModel::parent(const QModelIndex &child) const
{
    LibraryEntry* parent = entryFromIndex(child)->parent();

    if(!parent || parent == _root) {
        return QModelIndex();
    } else {
        return createIndex(parent->rowPosition(), 0, parent);
    }
}

int LibraryModel::rowCount(const QModelIndex &parent) const
{
    LibraryEntry* entry = entryFromIndex(parent);
    if(entry == nullptr) {
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

void LibraryModel::recordsForIndexes(const QModelIndexList &list, QList<QSqlRecord> &result) const
{
    QSet<QModelIndex> leafIndexes;
    for(const QModelIndex &index : list) {
        this->leafIndexes(index, leafIndexes);
    }

    for(const QModelIndex &index :leafIndexes) {
        result << entryFromIndex(index)->record();
    }
}

void LibraryModel::uidsForIndexes(const QModelIndexList &list, QStringList &result) const
{
    QSet<QModelIndex> leafIndexes;
    for(const QModelIndex &index : list) {
        this->leafIndexes(index, leafIndexes);
    }

    for(const QModelIndex &index : leafIndexes) {
        result << entryFromIndex(index)->record().value(Library::Uid).toString();
    }
}

void LibraryModel::leafIndexes(const QModelIndex &index, QSet<QModelIndex> &result) const
{
    int numChildren = rowCount(index);
    if(!numChildren) {
        result << index;
        return;
    }
    for(int i = 0 ; i < numChildren ; ++i) {
        QModelIndex child = index.child(i, index.column());
        if(rowCount(child)) {
            leafIndexes(child, result);
        } else {
            result << child;
        }
    }
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

const QModelIndexList LibraryModel::indexesForGroup(LibraryModel::GroupSelection group) const
{
    std::function<bool (const LibraryEntry*)> filter;
    switch((GroupSelection) group) {
    case AllTracks:
        filter = [](const LibraryEntry* entry) {
            return true;
        };
        break;
    case NewTracks:
        filter = [](const LibraryEntry* entry) {
            Library::FileStatus status = (Library::FileStatus) entry->record().value(Library::Status).toInt();
            return status.testFlag(Library::New);
        };
        break;
    case MissingTracks:
        filter = [](const LibraryEntry* entry) {
            Library::FileStatus status = (Library::FileStatus) entry->record().value(Library::Status).toInt();
            return status.testFlag(Library::FileNotFound);
        };
        break;
    case LinkedTracks:
        filter = [](const LibraryEntry* entry) {
            Library::FileStatus status = (Library::FileStatus) entry->record().value(Library::Status).toInt();
            bool hasLikedResult = !entry->record().value(Library::Bpid).isNull();
            return status.testFlag(Library::Searched) && hasLikedResult;
        };
        break;
    case SearchedAndNotLinkedTracks:
        filter = [](const LibraryEntry* entry) {
            Library::FileStatus status = (Library::FileStatus) entry->record().value(Library::Status).toInt();
            bool hasLikedResult = !entry->record().value(Library::Bpid).isNull();
            return status.testFlag(Library::Searched) && !hasLikedResult;
        };
        break;
    default:
        return QModelIndexList();
    }
    QModelIndexList indexesList;
    for(int i = 0 ; i < rowCount() ; ++i) {
        filteredIndexes(index(i, 0, QModelIndex()), filter, indexesList);
    }

    return indexesList;
}

void LibraryModel::updateSettings()
{
    QSettings settings;
    bool colorsEnabled =
            settings.value(TYM_PATH_DISPLAY_COLORS, TYM_DEFAULT_DISPLAY_COLORS).toBool();

    QVector<int> changes;
    if(colorsEnabled != _colorsEnabled) {
        changes << Qt::BackgroundColorRole;
        _colorsEnabled = colorsEnabled;
    }

    // If other settings have to been handled here, add more roles to changes

    if( ! changes.isEmpty()) {
        const QModelIndex invalid;
        emit dataChanged(index(0, columnCount(invalid) - 1, invalid),
                         index(rowCount(invalid)-1, columnCount(invalid)-1, invalid),
                         changes);
    }
}
void LibraryModel::refresh()
{
    if( ! _elementsList.exec()) {
        LOG_ERROR(tr("Unable to refresh library model : %1")
                  .arg(_elementsList.lastError().text()));
    } else {
        // Invalidate all existing ModelIndexes
        beginResetModel();

        // All items are deleted
        delete _dirMap["/"];

        // Reset the directories map and checked entries
        _dirMap.clear();

        LibraryEntry* systemRootEntry = new LibraryEntry();
        _dirMap["/"] = systemRootEntry;

        // _elementsList.size() always returns -1 on Linux, we must determine the
        // number of results manually
        int resultsCount = 0;
        while(_elementsList.next()) {
            // Helper to extract some informations from file path
            QFileInfo fileInfo(_elementsList.value(Library::FilePath).toString());
            QString filePath = fileInfo.exists() ?
                        fileInfo.canonicalPath() : fileInfo.absolutePath();

            LibraryEntry* parentDir = getLibraryNode(filePath);
            new LibraryEntry(_elementsList.record(), parentDir);
            resultsCount++;
        }
        _elementsList.finish();

        // Update _root pointer
        LibraryEntry *r = systemRootEntry;
        #ifdef Q_OS_WIN
            //: On Windows only, defines the system top level folder (above disks)
            QString path = tr("My Computer");
        #else
            QString path = "";
        #endif
        // From system root path to the first node with more than 1 children
        while(r->rowCount() == 1) {
            r = r->child(0);
            path.append(QDir::separator()).append(r->dirName());
        }
        // 'r' is the current valid root in the tree
        _root = r;

        // Update label above library view
        if(resultsCount < 1) {
            //: Used as displayed root dir when library is empty (no tracks in the database)
            emit rootPathChanged(tr("<empty>"));
        } else {
            emit rootPathChanged(path);
        }

        // Inform view that new items are stored in the model. It will
        // start te rebuild its ModelIndexes
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
        currentDirEntry = new LibraryEntry(currentDir.path().remove(QRegularExpression("/$")),
                                           _dirMap["/"]);
    } else {
        QDir upDir(currentDir);
        upDir.cdUp();
        LibraryEntry *parentEntry = getLibraryNode(upDir.canonicalPath());

        currentDirEntry = new LibraryEntry(currentDir.dirName(), parentEntry);
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

void LibraryModel::filteredIndexes(const QModelIndex &index,
                                   const std::function<bool (const LibraryEntry *)> &filterFunction,
                                   QModelIndexList &result) const
{
    int numChildren = rowCount(index);
    if(numChildren) {
        bool allChildrenAdded = true;
        for(int i = 0 ; i < numChildren ; ++i) {
            QModelIndex child = index.child(i, index.column());
            filteredIndexes(child, filterFunction, result);
            if(allChildrenAdded && !result.contains(child)) {
                allChildrenAdded = false;
            }
        }
        // All children have been added, add the parent
        if(allChildrenAdded) {
            result << index;
        }
    } else {
        if(filterFunction(entryFromIndex(index))) {
            result << index;
        }
    }
}
