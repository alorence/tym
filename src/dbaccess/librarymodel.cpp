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

const int LibraryModel::CHECKABLECOLUMN = Library::Name;

LibraryModel::LibraryModel(QObject *parent) :
    QAbstractItemModel(parent),
    _root(NULL),
    _db("libraryModel", this),
    _elementsList("SELECT * FROM LibraryHelper ORDER BY filePath", _db.dbObject())

{
    _headers << tr("Name") << tr("Status") << tr("Results") << tr("Infos");

    _noResultsColor = QColor(Qt::gray);
    _noResultsColor.setAlpha(30);
    _missingColor = QColor(Qt::red);
    _missingColor.setAlpha(60);
    _searchedColor = QColor(254, 183, 11, 50);
    _trackLinkedColor = QColor(Qt::green);
    _trackLinkedColor.setAlpha(50);

    updateSettings();
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    if(index.column() == CHECKABLECOLUMN) {
        Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);
        return  defaultFlags | Qt::ItemIsUserCheckable;
    }
    return QAbstractItemModel::flags(index);
}

QVariant LibraryModel::data(const QModelIndex &item, int role) const
{
    QSet<Qt::ItemDataRole> enabledRoles;
    enabledRoles << Qt::DisplayRole << Qt::DecorationRole << Qt::CheckStateRole;

    if(_colorsEnabled) {
        enabledRoles << Qt::BackgroundColorRole;
    }

    if ( ! enabledRoles.contains((Qt::ItemDataRole)role) && !(role == UniquePathRole && entryFromIndex(item)->isDirNode()))
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
    } else if (role == UniquePathRole) {

        return _dirMap.key(entryFromIndex(item), "/");
    } else if (role == Qt::BackgroundColorRole) {
        // Set BG color for each row
        Library::FileStatus status = (Library::FileStatus) entry->record().value(Library::Status).toInt();

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

bool LibraryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role != Qt::CheckStateRole) {
        return false;
    }

    setChecked(entryFromIndex(index), value == Qt::Checked);
    emit dataChanged(QModelIndex(), QModelIndex(), QVector<int>() << Qt::CheckStateRole);
    emit checkedItemsUpdated(_checkedEntries.size());
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
    for(const LibraryEntry* entry : _checkedEntries) {
        if(!entry->isDirNode()) {
            result << entry->record();
        }
    }
    return result;
}

QStringList LibraryModel::checkedUids() const
{
    QStringList result;
    for(const LibraryEntry* entry : _checkedEntries) {
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

void LibraryModel::updateSettings()
{
    QSettings settings;
    bool colorsEnabled = settings.value(TYM_PATH_DISPLAY_COLORS, TYM_DEFAULT_DISPLAY_COLORS).toBool();

    QVector<int> changes;
    if(colorsEnabled != _colorsEnabled)
        changes << Qt::BackgroundColorRole;

    _colorsEnabled = colorsEnabled;

    if( ! changes.isEmpty()) {
        emit dataChanged(index(0, columnCount(QModelIndex()) - 1, QModelIndex()),
                         index(rowCount(QModelIndex()) - 1, columnCount(QModelIndex()) - 1, QModelIndex()),
                         changes);
    }
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

        LibraryEntry* systemRootEntry = new LibraryEntry();
        _dirMap["/"] = systemRootEntry;

        while(_elementsList.next()) {
            // Helper to extract some informations from file path
            QFileInfo fileInfo(_elementsList.value(Library::FilePath).toString());

            LibraryEntry* parentDir = getLibraryNode(fileInfo.canonicalPath());
            new LibraryEntry(_elementsList.record(), parentDir);
        }

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
        if(r != systemRootEntry) {
            emit rootPathChanged(path);
        } else {
            //: Used as displayed root dir when library is empty (no tracks in the database)
            emit rootPathChanged(tr("<empty>"));
        }

        // Inform view that new items are stored in the model. It will start te rebuild its ModelIndexes
        endResetModel();
    }
}

void LibraryModel::checkSpecificGroup(int checkGroup)
{
    _checkedEntries.clear();

    std::function<bool (const LibraryEntry*)> filter;
    switch((GroupSelection) checkGroup) {
    case AllTracks:
        for(int i = 0 ; i < _root->rowCount() ; ++i) {
            setChecked(_root, true, true);
        }
        emit dataChanged(QModelIndex(), QModelIndex(), QVector<int>() << Qt::CheckStateRole);
        emit checkedItemsUpdated(_checkedEntries.size());
        return;
    case Neither:
        // _checkedEntries has been cleared, only emits signals to inform others
        emit dataChanged(QModelIndex(), QModelIndex(), QVector<int>() << Qt::CheckStateRole);
        emit checkedItemsUpdated(0);
        return;
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
        break;
    }

    recursiveFilteredSetChecked(_root, filter);
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

void LibraryModel::recursiveFilteredSetChecked(const LibraryEntry *entry, const std::function<bool (const LibraryEntry *)> &filterFunction)
{
    if(entry->rowCount()) {
        for(int i = 0 ; i < entry->rowCount() ; ++i) {
            recursiveFilteredSetChecked(entry, filterFunction);
        }
    } else {
        if(filterFunction(entry)) {
            setChecked(entry, true, false);
            emit dataChanged(QModelIndex(), QModelIndex(), QVector<int>() << Qt::CheckStateRole);
            emit checkedItemsUpdated(_checkedEntries.size());
        }
    }
}

void LibraryModel::setChecked(const LibraryEntry *entry, bool checked, bool recursive)
{
    // Check rows not already checked
    if(checked && !_checkedEntries.contains(entry)) {
        // Check the entry
        _checkedEntries.insert(entry);

        // Check entry's parent if it is not root and all sibling are checked
        if(entry->parent() != _root) {
            bool allChecked = true;
            for(const LibraryEntry* sibling : entry->parent()->children()) {
                if(!_checkedEntries.contains(sibling)) {
                    allChecked = false;
                    break;
                }
            }
            if(allChecked) {
                setChecked(entry->parent(), true, false);
            }
        }
    }
    // Uncheck rows already checked
    else if (!checked && _checkedEntries.contains(entry)) {
        // Uncheck the entry
        _checkedEntries.remove(entry);

        // Uncheck its parent if it was checked
        if(_checkedEntries.contains(entry->parent())){
            setChecked(entry->parent(), false, false);
        }
    }

    // If entry is a dir, update all its children recursively
    if(recursive && entry->isDirNode()) {
        for(const LibraryEntry* child : entry->children()) {
            setChecked(child, checked);
        }
    }
}

bool LibraryModel::isChecked(const QModelIndex &index) const
{
    return _checkedEntries.contains(entryFromIndex(index));
}

