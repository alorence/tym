#include "librarymodel.h"

LibraryModel::LibraryModel(QObject *parent, QSqlDatabase db) :
    QSqlRelationalTableModel(parent, db)
{
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    if(index.column() == 1) {
        return Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsDropEnabled | Qt::ItemIsSelectable;
    } else {
        return QSqlRelationalTableModel::flags(index);
    }
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
    if(index.column() == 1) {
        if(role == Qt::CheckStateRole) {
            return checkedRows.contains(index.row()) ? Qt::Checked : Qt::Unchecked;
        } else if (role == Qt::DisplayRole) {
            QString filePath = QSqlRelationalTableModel::data(index, role).toString();
            return QVariant(filePath.split(QDir::separator()).last());
        }
    }
    if(role == Qt::ToolTipRole) {
            QStringList elts = QSqlRelationalTableModel::data(QAbstractTableModel::index(index.row(), 1, index.parent()), Qt::DisplayRole).toString().split(QDir::separator());
            QString folder = QStringList(elts.mid(0, elts.size() - 2)).join(QDir::separator());

            QString tooltip = "In directory ";
            tooltip.append(folder);
            return QVariant(tooltip);
    }
    return QSqlRelationalTableModel::data(index, role);
}

bool LibraryModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() == 1 && role == Qt::CheckStateRole) {
        // Inform view that a chackbox has been checked / unchecked (call setRowSelectState)
        emit rowChecked(index.row(), value == Qt::Checked);
        return true;
    }
    else {
        return QSqlRelationalTableModel::setData(index, value, role);
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
