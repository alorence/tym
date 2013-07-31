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

#ifndef TRACKINFOSVIEW_H
#define TRACKINFOSVIEW_H

#include <QtWidgets>
#include <QtCore>
#include <QtSql>

namespace Ui {
class TrackInfosView;
}

class TrackInfosView : public QWidget
{
    Q_OBJECT
    
public:
    explicit TrackInfosView(QWidget *parent = 0);
    ~TrackInfosView();

    /*!
     * \brief Call ui->retranslateUi() when the current QTranslator change
     */
    void changeEvent(QEvent *) override;

public slots:
    /*!
     * \brief Delete all information from the widget. Clear each inputs, remove the picture,
     * reset member variables.
     */
    void clearData();
    /*!
     * \brief Update informations in the widget, from the given record.
     * \param record A QSqlRecord containg all information from a file / track
     */
    void updateInfos(QSqlRecord record);
    /*!
     * \brief Check if given picture id is the one corresponding to information currently
     * displayed by the widget. If yes, display the picture after loading it from hard drive.
     * \param picId
     */
    void displayDownloadedPicture(QString picId);

signals:
    /*!
     * \brief Signal emmitted when a new track is loaded, and its corresponding picture
     * is not already stored in the local hard drive.
     * \param The id of the track
     */
    void needDownloadPicture(const QString &picId);

private:
    Ui::TrackInfosView *ui;

    QString _currentPictureId;
    QPixmap _defaultAlbumArt;

    QSqlRecord _result;
};

#endif // TRACKINFOSVIEW_H
