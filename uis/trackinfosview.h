/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
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

#ifndef TRACKINFOSVIEW_H
#define TRACKINFOSVIEW_H

#include <QtGui>
#include <QtCore>
#include <QtSql>

#include "src/bpdatabase.h"

namespace Ui {
class TrackInfosView;
}

class TrackInfosView : public QWidget
{
    Q_OBJECT
    
public:
    explicit TrackInfosView(QWidget *parent = 0);
    ~TrackInfosView();

public slots:
    void clearData();
    void updateInfos(QSqlRecord);
    void displayDownloadedPicture(QString url);

signals:
    void downloadPicture(const QString &picId);

private:
    Ui::TrackInfosView *ui;

    QString currentPictureId;
};

#endif // TRACKINFOSVIEW_H
