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

#include "picturedownloader.h"

#include <Logger.h>

#include "commons.h"

PictureDownloader::PictureDownloader(QObject *parent) :
    QObject(parent),
    _manager(new QNetworkAccessManager(this))
{
}

PictureDownloader::~PictureDownloader()
{
    delete _manager;
}

void PictureDownloader::downloadTrackPicture(const QString & picId)
{
    if(_downloadManagaer.values().contains(picId)) return;

    QNetworkRequest request(Constants::dynamicPictureUrl().arg(picId));

    QNetworkReply *reply = _manager->get(request);
    _downloadManagaer.insert(reply, picId);
    connect(reply, SIGNAL(readyRead()), this, SLOT(writeTrackPicture()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(requestError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(sendFinalNotification()));
}

void PictureDownloader::writeTrackPicture() const
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    QString imgName = _downloadManagaer.value(reply) + ".jpg";
    QFile imgFile(Constants::picturesLocation() + QDir::separator() + imgName);

    imgFile.open(QIODevice::WriteOnly | QIODevice::Append);
    imgFile.write(reply->readAll());
    imgFile.close();
}

void PictureDownloader::sendFinalNotification()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    emit pictureDownloadFinished(_downloadManagaer.take(reply));
    reply->deleteLater();
}


void PictureDownloader::requestError(QNetworkReply::NetworkError error) const
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    LOG_WARNING(tr("Error on request %1 : %2")
                .arg(reply->request().url().toString())
                .arg(error));
    reply->deleteLater();
}
