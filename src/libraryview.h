#ifndef LIBRARYVIEW_H
#define LIBRARYVIEW_H

#include <QtCore>
#include <QtGui>
#include <QtSql>
#include "libraryview.h"

class LibraryView : public QTableView
{
    Q_OBJECT
public:
    explicit LibraryView(QWidget *parent = 0);
    void setModel(QAbstractItemModel *model);

signals:
    void rowSelectedChanged(QList<int>);

private slots:
    void rowCountChanged(int, int);

private slots:
    void onSelectionChanged(QItemSelection, QItemSelection);
    void setRowSelectState(int row, bool state);
};

#endif // LIBRARYVIEW_H
