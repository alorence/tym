/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag your Library).
*
* TYM (Tag your Library) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag your Library) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag your Library).  If not, see <http://www.gnu.org/licenses/>.
*/

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
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

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
