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
    // For tooltip, display onlt the folder (all text before the last dir separator)
    if(role == Qt::ToolTipRole) {
            QStringList pathElements = QSqlRelationalTableModel::data(QAbstractTableModel::index(index.row(), 1, index.parent()), Qt::DisplayRole).toString().split(QDir::separator());
            pathElements.removeLast();

            QString tooltip = "In directory ";
            tooltip.append(pathElements.join(QDir::separator()));
            return QVariant(tooltip);
    }
    return QSqlRelationalTableModel::data(index, role);
}

bool LibraryModel::setData(const QModelIndex &ind, const QVariant &value, int role)
{
    if(ind.column() == 1 && role == Qt::CheckStateRole) {
        checkedRows << ind.row();
        QItemSelectionModel::SelectionFlag selStatus = value == Qt::Checked ? QItemSelectionModel::Select : QItemSelectionModel::Deselect;
        for(int i = 0 ; i < columnCount() ; i++) {
            // Create index for complete line
            QModelIndex lineIndex = index(ind.row(), i, ind.parent());
            // Select corresponding row in QTableView
            emit rowChecked(lineIndex, selStatus);
        }
        return true;
    }
    else {
        return QSqlRelationalTableModel::setData(ind, value, role);
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

void LibraryModel::deleteSelected()
{
    QList<int> checkedCopy = checkedRows;
    for(int i = checkedCopy.size() - 1 ; i >= 0 ; --i) {
        if( ! removeRow(checkedCopy.value(i))) {
            qWarning() << "Unable to delete row" << checkedCopy.value(i) << ":" << lastError().text();
        }
    }
}

void LibraryModel::updateCheckedRows(const QItemSelection& selected, const QItemSelection& deselected)
{
    foreach(QItemSelectionRange range, deselected) {
        foreach(QModelIndex index, range.indexes()) {
            checkedRows.removeAll(index.row());
        }
    }
    foreach(QItemSelectionRange range, selected) {
        foreach(QModelIndex index, range.indexes()) {
            checkedRows << index.row();
        }
    }
    emit dataChanged(index(0, 0), index(rowCount() - 1, 0));
}
