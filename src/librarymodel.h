#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "commons.h"

class LibraryModel : public QSqlRelationalTableModel
{
    Q_OBJECT


public:
    explicit LibraryModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &i, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QList<int> selectedIds() const;
    QList<QPair<int, QSqlRecord> > selectedRecords() const;

public slots:
    void updateCheckedRows(const QItemSelection &, const QItemSelection &);
    void deleteSelected();
    void refreshAndPreserveSelection();

private:
    QList<int> checkedRows;
    int columnWithCheckbox;

signals:
    void rowCheckedOrUnchecked(QItemSelection,QItemSelectionModel::SelectionFlags);

};

#endif // LIBRARYMODEL_H
