#include "searchresultsmodel.h"

SearchResultsModel::SearchResultsModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db),
    tick(QPixmap(":/img/actions/tick")),
    transp(QPixmap(tick.width(), tick.height()))
{
    transp.fill(Qt::transparent);
    columnWithTicker = SearchResultsIndexes::Track;
}

QVariant SearchResultsModel::data(const QModelIndex &item, int role) const
{
    if(role == Qt::DecorationRole && item.column() == columnWithTicker) {

        if(QSqlTableModel::data(index(item.row(), SearchResultsIndexes::DefaultFor, item.parent())).toBool()) {
            return tick;
        } else {
            return transp;
        }
    }
    return QSqlTableModel::data(item, role);
}

void SearchResultsModel::refresh(QString libId)
{
    if(filter().endsWith(libId)) {
        select();
    }
}
