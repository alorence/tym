#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QtCore>
#include <QtGui>
#include <QtSql>

class LibraryModel : public QSqlRelationalTableModel
{
    Q_OBJECT


public:
    explicit LibraryModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &item, int role) const;
    bool setData(const QModelIndex &i, const QVariant &value, int role);

    QList<int> selectedIds() const;
    QList<QPair<int, QSqlRecord> > selectedRecords() const;

public slots:
    void deleteSelected();
    void updateCheckedRows(const QItemSelection &, const QItemSelection &);

private:
    QList<int> checkedRows;

signals:
    void rowChecked(QModelIndex,QItemSelectionModel::SelectionFlags);

};

#endif // LIBRARYMODEL_H
