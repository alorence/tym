#ifndef LIBRARYVIEW_H
#define LIBRARYVIEW_H

#include <QtGui>
#include <QtSql>

class LibraryView : public QTableView
{
    Q_OBJECT
public:
    explicit LibraryView(QWidget *parent = 0);
    void setModel(QAbstractItemModel *model);

signals:
    void rowSelectedChanged(QList<int>);

private slots:
    void onSelectionChanged(QItemSelection, QItemSelection);
    void setRowSelectState(int row, bool state);
};

#endif // LIBRARYVIEW_H
