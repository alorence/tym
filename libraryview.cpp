#include "libraryview.h"

LibraryView::LibraryView(QWidget *parent) :
    QTableView(parent)
{
    setShowGrid(false);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

void LibraryView::setModel(QAbstractItemModel *model)
{
    QTableView::setModel(model);
    resizeColumnsToContents();
    verticalHeader()->setVisible(false);

    connect(selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onSelectionChanged(QItemSelection,QItemSelection)));
}


void LibraryView::onSelectionChanged(QItemSelection, QItemSelection)
{
    QList<int> selectedRows;
    foreach(QModelIndex index, selectionModel()->selectedRows()) {
        selectedRows << index.row();
    }
    emit rowSelectedChanged(selectedRows);
}

void LibraryView::setRowSelectState(int row, bool state)
{
    const QAbstractItemModel *model = selectionModel()->model();
    QItemSelection selection(model->index(row, 0), model->index(row, model->columnCount() - 1));
    selectionModel()->select(selection, state ? QItemSelectionModel::Select : QItemSelectionModel::Deselect);
}
