/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag Your Music) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag Your Music) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag Your Music).  If not, see <http://www.gnu.org/licenses/>.
*/

#include "searchresultsmodel.h"

#include "commons.h"

SearchResultsModel::SearchResultsModel(QObject *parent, QSqlDatabase db) :
    QSqlTableModel(parent, db),
    tick(QPixmap(":/img/actions/tick")),
    transp(QPixmap(tick.width(), tick.height()))
{
    transp.fill(Qt::transparent);
    columnWithTicker = SearchResults::Track;
}

QVariant SearchResultsModel::data(const QModelIndex &item, int role) const
{
    if(role == Qt::DecorationRole && item.column() == columnWithTicker) {

        if(QSqlTableModel::data(index(item.row(), SearchResults::DefaultFor, item.parent())).toBool()) {
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
