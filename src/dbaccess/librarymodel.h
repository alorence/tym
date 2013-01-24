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

#ifndef LIBRARYMODEL_H
#define LIBRARYMODEL_H

#include <QtCore>
#include <QtSql>

class LibraryModel : public QSqlTableModel
{
    Q_OBJECT


public:
    explicit LibraryModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &i, const QVariant &value, int role);
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    QSet<int> selectedIds() const;
    QHash<int, QSqlRecord> selectedRecords() const;

signals:
    void rowCheckedOrUnchecked(const QModelIndex&,QItemSelectionModel::SelectionFlags);
    void rowCheckedOrUnchecked(const QItemSelection&,QItemSelectionModel::SelectionFlags);

public slots:
    void updateCheckedRows(const QItemSelection &, const QItemSelection &);
    void refresh(const QString &bpid = QString());
    void refresh(int row);
    void unselectRowsAndRefresh(QList<int> rows);

private:
    QSet<int> checkedRows;
    int columnWithCheckbox;

};

#endif // LIBRARYMODEL_H
