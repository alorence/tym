#ifndef SEARCHRESULTSMODEL_H
#define SEARCHRESULTSMODEL_H

#include <QtCore>
#include <QtGui>
#include <QtSql>

#include "commons.h"

class SearchResultsModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit SearchResultsModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;


signals:

public slots:
    void refresh(QString libId);

private:
    int columnWithTicker;

    QPixmap tick;
    QPixmap transp;
};

#endif // SEARCHRESULTSMODEL_H
