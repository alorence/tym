#include "librarymodel.h"

LibraryModel::LibraryModel(QObject *parent, QSqlDatabase db) :
    QSqlRelationalTableModel(parent, db)
{
    columnWithCheckbox = LibraryIndexes::FilePath;
}

Qt::ItemFlags LibraryModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flags = QSqlRelationalTableModel::flags(index);
    if(index.column() == columnWithCheckbox) {
        return flags | Qt::ItemIsUserCheckable;
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

            QString filePath = QSqlRelationalTableModel::data(ind, role).toString();
            return QVariant(filePath.split(QDir::separator()).last());

        } else if(role == Qt::ToolTipRole) {
            // Display only the folder (all text before the last dir separator)
            QStringList pathElements = QSqlRelationalTableModel::data(QAbstractTableModel::index(ind.row(), LibraryIndexes::FilePath, ind.parent()), Qt::DisplayRole)
                    .toString().split(QDir::separator());
            pathElements.removeLast();

            QString tooltip = "In directory ";
            tooltip.append(pathElements.join(QDir::separator()));
            return QVariant(tooltip);

        }
    } else if (ind.column() == LibraryIndexes::Note && role == Qt::DisplayRole) {

        int status = QSqlRelationalTableModel::data(index(ind.row(), LibraryIndexes::Status), Qt::DisplayRole).toInt();

        switch(status) {
        case TrackStates::New:
            return tr("Recently imported, no result for now...");
        break;
        case TrackStates::FileNotFound:
            return tr("Unable to find the file on your disk.");
        break;
        case TrackStates::ResultsAvailable:
            return tr("n results available.");
        break;
        default:
            return "";
        }

    } else if (ind.column() == LibraryIndexes::Status && role == Qt::DisplayRole) {

        int status = QSqlRelationalTableModel::data(index(ind.row(), LibraryIndexes::Status), Qt::DisplayRole).toInt();

        switch (status) {
        case TrackStates::New:
            return tr("New");
        break;
        case TrackStates::FileNotFound:
            return tr("Missing");
        break;
        case TrackStates::ResultsAvailable:
            return tr("Searched");
        break;
        default:
            return "";
        }
    }

    return QSqlRelationalTableModel::data(ind, role);
}

bool LibraryModel::setData(const QModelIndex &ind, const QVariant &value, int role)
{
    if(ind.column() == columnWithCheckbox && role == Qt::CheckStateRole) {
        QItemSelectionModel::SelectionFlag selStatus;
        selStatus = value == Qt::Checked ? QItemSelectionModel::Select : QItemSelectionModel::Deselect;
        QItemSelection lineSelection(index(ind.row(), 0, ind.parent()), index(ind.row(), columnCount() - 1, ind.parent()));
        emit rowCheckedOrUnchecked(lineSelection, selStatus);
        return true;
    }
    else {
        return QSqlRelationalTableModel::setData(ind, value, role);
    }
}

QVariant LibraryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section == LibraryIndexes::Note && orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return "message";
    }
    return QSqlRelationalTableModel::headerData(section, orientation, role);
}

int LibraryModel::columnCount(const QModelIndex &) const
{
    return QSqlRelationalTableModel::columnCount() + 1;
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
        setData(index(row, columnWithCheckbox), Qt::Checked, Qt::CheckStateRole);
    }
}

