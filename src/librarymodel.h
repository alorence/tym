#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QtSql>
#include <QtCore>

class LibraryModel : public QSqlRelationalTableModel
{
    Q_OBJECT


public:
    explicit LibraryModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &item, int role) const;
    bool setData(const QModelIndex &i, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QList<int> getSelectedIds() const;
    QList<QPair<int, QSqlRecord> > getSelectedRecords() const;



public slots:
    void importFile(QString filePath);
    void importFiles(const QStringList &);
    void setRowsChecked(QList<int> rows);

private:
    QList<int> checkedRows;

signals:
    void rowChecked(int row, bool state);

};

#endif // LIBRARYMODEL_H
