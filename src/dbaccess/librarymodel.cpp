/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag Your Music) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag Your Music) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag Your Music).  If not, see <http://www.gnu.org/licenses/>.
*/

#include "librarymodel.h"

LibraryModel::LibraryModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db)
{
    columnWithCheckbox = LibraryIndexes::FilePath;
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    if(index.column() == columnWithCheckbox) {
        return flags | Qt::ItemIsUserCheckable;
    } else if(index.column() == LibraryIndexes::Message){
        return QSqlTableModel::flags(QAbstractTableModel::index(index.row(), LibraryIndexes::FilePath, index.parent()));
    } else {
        return flags;
    }
}

QVariant LibraryModel::data(const QModelIndex &ind, int role) const
{
    if(ind.column() == columnWithCheckbox) {
        if(role == Qt::CheckStateRole) {

            return checkedRows.contains(ind.row()) ? Qt::Checked : Qt::Unchecked;

        } else if (role == Qt::DisplayRole) {

            QString filePath = QSqlTableModel::data(ind, role).toString();
            return QVariant(filePath.split(QDir::separator()).last());

        } else if(role == Qt::ToolTipRole) {
            // Display only the folder (all text before the last dir separator)
            QStringList pathElements = QSqlTableModel::data(QAbstractTableModel::index(ind.row(), LibraryIndexes::FilePath, ind.parent()), Qt::DisplayRole)
                    .toString().split(QDir::separator());
            pathElements.removeLast();

            QString tooltip = tr("In directory ");
            tooltip.append(pathElements.join(QDir::separator()));
            return QVariant(tooltip);

        }
    } else if (ind.column() == LibraryIndexes::Message && role == Qt::DisplayRole) {

        int status = QSqlTableModel::data(index(ind.row(), LibraryIndexes::Status), Qt::DisplayRole).toInt();
        int n = 0;

        switch(status) {
        case FileStatus::New:
            return tr("Recently imported, no result for now...");
        break;
        case FileStatus::FileNotFound:
            return tr("Unable to find the file on your disk.");
        break;
        case FileStatus::ResultsAvailable:
            n = QSqlTableModel::data(index(ind.row(), LibraryIndexes::Message), Qt::DisplayRole).toInt();
            return tr("%n result(s) available.", "Display number of results available for one library element", n);
        break;
        default:
            return "";
        }

    } else if (ind.column() == LibraryIndexes::Status && role == Qt::DisplayRole) {

        int status = QSqlTableModel::data(index(ind.row(), LibraryIndexes::Status), Qt::DisplayRole).toInt();

        switch (status) {
        case FileStatus::New:
            return tr("New");
        break;
        case FileStatus::FileNotFound:
            return tr("Missing");
        break;
        case FileStatus::ResultsAvailable:
            return tr("Searched");
        break;
        default:
            return "";
        }
    }
    return QSqlTableModel::data(ind, role);
}

bool LibraryModel::setData(const QModelIndex &ind, const QVariant &value, int role)
{
    if(ind.column() == columnWithCheckbox && role == Qt::CheckStateRole) {
        QItemSelectionModel::SelectionFlag selStatus;
        selStatus = value == Qt::Checked ? QItemSelectionModel::Select : QItemSelectionModel::Deselect;
        QItemSelection line(index(ind.row(), 0, ind.parent()), index(ind.row(), columnCount() - 1, ind.parent()));
        emit rowCheckedOrUnchecked(line, selStatus);
        emit rowCheckedOrUnchecked(index(ind.row(), columnWithCheckbox, ind.parent()), selStatus);
        return true;
    }
    else {
        return QSqlTableModel::setData(ind, value, role);
    }
}

QVariant LibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch(section) {
        case LibraryIndexes::Uid:           return tr("Uid");
        case LibraryIndexes::FilePath:      return tr("File");
        case LibraryIndexes::Status:        return tr("Status");
        case LibraryIndexes::Message:       return tr("Comment");
        case LibraryIndexes::Bpid:          return tr("Track Id");
        }
    }
    return QSqlTableModel::headerData(section, orientation, role);
}

bool LibraryModel::select()
{
    QMutexLocker locker(BPDatabase::instance()->dbMutex);
    return QSqlTableModel::select();
}

void LibraryModel::updateCheckedRows(const QItemSelection& selected, const QItemSelection& deselected)
{
    int i;
    QItemSelectionRange range;
    foreach(range, deselected) {
        for(i = range.top() ; i <= range.bottom() ; i++) {
            checkedRows.removeAll(i);
        }
        emit dataChanged(index(range.top(), columnWithCheckbox), index(range.bottom(), columnWithCheckbox));
    }
    foreach(range, selected) {
        for(i = range.top() ; i <= range.bottom() ; i++) {
            checkedRows << i;
        }
        emit dataChanged(index(range.top(), columnWithCheckbox), index(range.bottom(), columnWithCheckbox));
    }
}

QList<int> LibraryModel::selectedIds() const
{
    return checkedRows;
}

QList<QPair<int, QSqlRecord> > LibraryModel::selectedRecords() const
{
    QList<QPair<int, QSqlRecord> > result;
    foreach(int index, checkedRows) {
        result << QPair<int, QSqlRecord>(index, record(index));
    }
    return result;
}

void LibraryModel::refresh()
{
    QList<int> checkedCopy = checkedRows;
    select();
    foreach(int row, checkedCopy) {
        setData(index(row, columnWithCheckbox), Qt::Checked, Qt::CheckStateRole);
    }
}

void LibraryModel::unselectRowsAndRefresh(QList<int> rows)
{
    foreach(int row, rows) {
        checkedRows.removeAll(row);
    }
    refresh();
}
