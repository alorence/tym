/*****************************************************************************
Copyright 2012-2013 Antoine Lorence. All right reserved.

This file is part of TYM (Tag Your Music).

TYM (Tag Your Music) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TYM (Tag Your Music) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TYM (Tag Your Music). If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef SEARCHRESULTSMODEL_H
#define SEARCHRESULTSMODEL_H

#include <QtCore>
#include <QtGui>
#include <QtSql>

/*!
 * \brief A simple inherited QSqlTableModel, which display images on 1 row.
 */
class SearchResultsModel : public QSqlTableModel
{
    Q_OBJECT
public:
    /*!
     * \brief Create the model.
     * This constructor initialize images (ticker) returned to the view.
     * \param parent
     * \param db
     */
    explicit SearchResultsModel(QObject *parent = 0, QSqlDatabase db = QSqlDatabase());
    /*!
     * \brief Reimplemented from QSqlTableModel::data() to display an image on a result linked
     * \sa QSqlTableModel::data()
     */
    QVariant data(const QModelIndex &item, int role = Qt::DisplayRole) const override;

public slots:
    /*!
     * \brief Refresh the search result model content, if necessary.
     * Read current filter() to check if results displayed are for this ID. If yes, the content is refreshed.
     * \param libId Library ID of element which has just been updated
     * \sa QSqlTableModel::select()
     * \sa QSqlTableModel::filter()
     */
    void refresh(const QString & libId);

private:
    int _columnWithTicker;

    QPixmap _tick;
    QPixmap _transp;
};

#endif // SEARCHRESULTSMODEL_H
