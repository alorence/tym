#include "searchresultsmodel.h"

SearchResultsModel::SearchResultsModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db)
{
}
