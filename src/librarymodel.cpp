#include "librarymodel.h"

LibraryModel::LibraryModel(QObject *parent, QSqlDatabase db) :
    QSqlRelationalTableModel(parent, db)
{
    columnWithCheckbox = 1;
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    if(index.column() == columnWithCheckbox) {
        return Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable | Qt::ItemIsDropEnabled | Qt::ItemIsSelectable;
    } else {
        return QSqlRelationalTableModel::flags(index);
    }
}

QVariant LibraryModel::data(const QModelIndex &index, int role) const
{
    if(index.column() == columnWithCheckbox) {
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
    if(ind.column() == columnWithCheckbox && role == Qt::CheckStateRole) {
        QItemSelectionModel::SelectionFlag selStatus = value == Qt::Checked ? QItemSelectionModel::Select : QItemSelectionModel::Deselect;
        QItemSelection lineSelection(index(ind.row(), 0, ind.parent()), index(ind.row(), columnCount() -1, ind.parent()));
        emit rowChecked(lineSelection, selStatus);
        return true;
    }
    else {
        return QSqlRelationalTableModel::setData(ind, value, role);
    }
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

void LibraryModel::deleteSelected()
{
    QList<int> checkedCopy = checkedRows;
    qSort(checkedCopy.begin(), checkedCopy.end(), qGreater<int>());
    foreach(int row, checkedCopy) {
        if( ! removeRow(row)) {
            qWarning() << QString(tr("Unable to delete row %1 : %2")).arg(row).arg(lastError().text());
        }
    }
}

void LibraryModel::refreshAndPreserveSelection()
{
    QList<int> checkedCopy = checkedRows;
    select();
    foreach(int row, checkedCopy) {
        qDebug() << "check row" << row;
        setData(index(row, columnWithCheckbox), Qt::Checked, Qt::CheckStateRole);
    }
}

