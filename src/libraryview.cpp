#include "libraryview.h"

LibraryView::LibraryView(QWidget *parent) :
    QTableView(parent)
{
}

void LibraryView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    resizeColumnsToContents();

    // starting point of a selection changing
    connect(selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
            this, SLOT(onSelectedRowsChanged(const QItemSelection&,const QItemSelection&)));
}


void LibraryView::onSelectedRowsChanged(const QItemSelection&, const QItemSelection&)
{
    QList<int> selectedRows;
    foreach(QModelIndex index, selectionModel()->selectedRows()) {
        selectedRows << index.row();
    }
    // Inform model that a row has been selected (call setRowsChecked())
    emit rowSelectedChanged(selectedRows);
}

void LibraryView::setRowSelectState(int row, bool state)
{
    const QAbstractItemModel *model = selectionModel()->model();
    QItemSelection selection(model->index(row, 0), model->index(row, model->columnCount() - 1));
    selectionModel()->select(selection, state ? QItemSelectionModel::Select : QItemSelectionModel::Deselect);
}

void LibraryView::rowCountChanged(int, int)
{
    resizeColumnsToContents();
}


