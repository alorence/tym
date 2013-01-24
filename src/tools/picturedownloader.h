/**
* Copyright 2013 Antoine Lorence. All right reserved.
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

#ifndef PICTUREDOWNLOADER_H
#define PICTUREDOWNLOADER_H

#include <QObject>
#include <QtNetwork>

class PictureDownloader : public QObject
{
    Q_OBJECT
public:
    explicit PictureDownloader(QObject *parent = 0);
    ~PictureDownloader();

signals:
    void pictureDownloadFinished(const QString &picId) const;
    
public slots:
    void downloadTrackPicture(const QString &picId);

private slots:
    void writeTrackPicture() const;
    void sendFinalNotification();
    void requestError(QNetworkReply::NetworkError error) const;

private :
    QNetworkAccessManager * _manager;
    QMap<QNetworkReply*, QString> _downloadManagaer;
};

#endif // PICTUREDOWNLOADER_H
