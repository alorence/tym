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

#include <QtWidgets>

#include "commons.h"
#include "bpdatabase.h"

LibraryModel::LibraryModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db)
{
    columnWithCheckbox = Library::FilePath;
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlTableModel::flags(index);
    if(index.column() == columnWithCheckbox) {
        return flags | Qt::ItemIsUserCheckable;
    } else if(index.column() == Library::NumResults){
        return QSqlTableModel::flags(QAbstractTableModel::index(index.row(), Library::FilePath, index.parent()));
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
            QStringList pathElements = QSqlTableModel::data(QAbstractTableModel::index(ind.row(), Library::FilePath, ind.parent()), Qt::DisplayRole)
                    .toString().split(QDir::separator());
            pathElements.removeLast();

            QString tooltip = tr("In directory ");
            tooltip.append(pathElements.join(QDir::separator()));
            return QVariant(tooltip);

        }
    } else if (ind.column() == Library::NumResults && role == Qt::DisplayRole) {

        Library::FileStatus status = (Library::FileStatus) QSqlTableModel::data(index(ind.row(), Library::Status), Qt::DisplayRole).toInt();

        QString trackLinked;
        if( ! QSqlTableModel::data(index(ind.row(), Library::Bpid), Qt::DisplayRole).toString().isEmpty()) {
            trackLinked = " Entry linked to a track.";
        }

        if(status.testFlag(Library::FileNotFound)) {
            return tr("The file has been moved or renamed out of this software.%1").arg(trackLinked);
        } else if (status.testFlag(Library::New)) {
            return tr("No result for now...");
        } else if (status.testFlag(Library::ResultsAvailable)) {
            if(trackLinked.isEmpty()) {
                return tr("Select the better result on the right");
            } else {
                return tr("A result has been linked to this track.");
            }
        } else {
            return tr("");
        }

    } else if (ind.column() == Library::Status && role == Qt::DisplayRole) {

        Library::FileStatus status = (Library::FileStatus) QSqlTableModel::data(index(ind.row(), Library::Status), Qt::DisplayRole).toInt();

        if(status.testFlag(Library::FileNotFound)) {
            return tr("Missing");
        } else if (status.testFlag(Library::New)) {
            return tr("Freshly added");
        } else if (status.testFlag(Library::ResultsAvailable)) {
            int n = QSqlTableModel::data(index(ind.row(), Library::NumResults), Qt::DisplayRole).toInt();
            return tr("%n result(s)", "Display number of results available for one library element", n);
        } else {
            return tr("Unknown");
        }
    }
    return QSqlTableModel::data(ind, role);
}

bool LibraryModel::setData(const QModelIndex &ind, const QVariant &value, int role)
{
    if(ind.column() == columnWithCheckbox && role == Qt::CheckStateRole) {
        QItemSelectionModel::SelectionFlag selStatus;
        selStatus = value == Qt::Checked ? QItemSelectionModel::Select : QItemSelectionModel::Deselect;

        if(selStatus != data(ind, Qt::CheckStateRole)) {
            QItemSelection line(index(ind.row(), 0, ind.parent()), index(ind.row(), columnCount() - 1, ind.parent()));
            emit rowCheckedOrUnchecked(line, selStatus);

            emit rowCheckedOrUnchecked(index(ind.row(), columnWithCheckbox, ind.parent()), selStatus);
        }
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
        case Library::Uid:           return tr("Uid");
        case Library::FilePath:      return tr("File");
        case Library::Status:        return tr("Status");
        case Library::NumResults:       return tr("Comment");
        case Library::Bpid:          return tr("Track Id");
        }
    }
    return QSqlTableModel::headerData(section, orientation, role);
}

bool LibraryModel::select()
{
    return QSqlTableModel::select();
}

void LibraryModel::updateCheckedRows(const QItemSelection& selected, const QItemSelection& deselected)
{
    int i;
    QItemSelectionRange range;
    foreach(range, deselected) {
        for(i = range.top() ; i <= range.bottom() ; i++) {
            checkedRows.remove(i);
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

QSet<int> LibraryModel::selectedIds() const
{
    return checkedRows;
}

QHash<int, QSqlRecord> LibraryModel::selectedRecords() const
{
    QHash<int, QSqlRecord> result;
    foreach(int index, checkedRows) {
        result[index] = record(index);
    }
    return result;
}

void LibraryModel::refresh()
{
    select();
}

void LibraryModel::unselectRowsAndRefresh(QList<int> rows)
{
    foreach(int row, rows) {
        checkedRows.remove(row);
    }
    refresh();
}

