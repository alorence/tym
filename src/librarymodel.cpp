#include "librarymodel.h"

LibraryModel::LibraryModel(QObject *parent, QSqlDatabase db) :
    QSqlRelationalTableModel(parent, db)
{
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    if(index.column() == 0) {
        return Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsDropEnabled | Qt::ItemIsSelectable;
    } else {
        return QSqlRelationalTableModel::flags(index);
    }
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
    if(index.column() == 0) {
        if(role == Qt::CheckStateRole) {
            return checkedRows.contains(index.row()) ? Qt::Checked : Qt::Unchecked;
        }
        // For another role : no data displayed
        else return QVariant();
    }
    return QSqlRelationalTableModel::data(index, role);
}

bool LibraryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 0 && role == Qt::CheckStateRole) {
        // Inform view that a chackbox has been checked / unchecked (call setRowSelectState)
        emit rowChecked(index.row(), value == Qt::Checked);
        return true;
    }
    else
        return QSqlRelationalTableModel::setData(index, value, role);
}

QVariant LibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // Don't display header for column 0
    if(orientation == Qt::Horizontal && section == 0)
        return QVariant();
    else
        return QSqlRelationalTableModel::headerData(section, orientation, role);
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

void LibraryModel::setRowsChecked(QList<int> rows)
{
    checkedRows.swap(rows);
    // Inform view that state has changed (to refresh checkbox display)
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}

void LibraryModel::deleteSelected()
{
    QList<int> checkedCopy = checkedRows;
    for(int i = checkedCopy.size() - 1 ; i >= 0 ; --i) {
        if( ! removeRow(checkedCopy.value(i))) {
            qWarning() << "Unable to delete row" << checkedCopy.value(i) << ":" << lastError().text();
        }
    }
}
