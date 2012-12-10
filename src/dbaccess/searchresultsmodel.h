#ifndef SEARCHRESULTSMODEL_H
#define SEARCHRESULTSMODEL_H

#include <QSqlTableModel>

class SearchResultsModel : public QSqlTableModel
{
    Q_OBJECT
public:
    explicit SearchResultsModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

signals:

public slots:

};

#endif // SEARCHRESULTSMODEL_H
