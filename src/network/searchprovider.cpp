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

#include "searchprovider.h"
#include "src/commons.h"

SearchProvider::SearchProvider(SettingsDialog *sd, QObject *parent) :
    QObject(parent),
    manager(new QNetworkAccessManager(this)),
    settings(sd),
    replyMap(),
    textSearchMapper(new QSignalMapper(this))
{
    apiUrl = QUrl(QString("http://%1").arg(settings->getSettingsValue("settings/network/beatport/apihost").toString()));
    tracksPath = "/catalog/3/tracks";
    searchPath = "/catalog/3/search";

    connect(textSearchMapper, SIGNAL(mapped(QString)), this, SLOT(parseReplyForNameSearch(QString)));
}

SearchProvider::~SearchProvider()
{
    foreach(QNetworkReply* reply, replyMap.keys())
       delete replyMap.take(reply);

    delete textSearchMapper;
}

void SearchProvider::initProxy()
{
    /*
    QString h = settings.value("network/proxy/host").toString();
    qint16 p = settings.value("network/proxy/port").toInt();
    QString u = settings.value("network/proxy/user").toString();
    QString pwd = settings.value("network/proxy/pass").toString();
    QNetworkProxy proxy(QNetworkProxy::DefaultProxy, h, p, u, pwd);
    QNetworkProxy::setApplicationProxy(proxy);
    */
}

void SearchProvider::searchFromIds(QMap<QString, QString> * uidBpidMap)
{
    QUrl requestUrl = QUrl(apiUrl);
    requestUrl.setPath(tracksPath);

    QList<QPair<QString, QString> > queryItems = QList<QPair<QString, QString> >();
    queryItems << QPair<QString, QString>("ids", QStringList(uidBpidMap->values()).join(","));
    requestUrl.setQueryItems(queryItems);

    QNetworkRequest request(requestUrl);

    QNetworkReply *reply = manager->get(request);

    replyMap.insert(reply, uidBpidMap);

    connect(reply, SIGNAL(finished()), this, SLOT(parseReplyForIdSearch()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(getError(QNetworkReply::NetworkError)));
}

void SearchProvider::parseReplyForIdSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QMap<QString, QString> *uidBpidMap = replyMap.take(reply);

    QString jsonResponse(reply->readAll());
    QVariant response = QtJson::parse(jsonResponse);

    QMapIterator<QString, QString> req(*uidBpidMap);
    while(req.hasNext()) {
        req.next();

        QString uid = req.key();
        QString bpid = req.value();

        foreach(QVariant track, response.toMap()["results"].toList()) {
            if(bpid == track.toMap()["id"].toString()) {
                emit searchResultAvailable(uid, track);
                break;
            }
        }
    }

    delete uidBpidMap;
    reply->deleteLater();
}


void SearchProvider::searchFromName(QMap<QString, QString> *rowNameMap)
{
    QUrl requestUrl = QUrl(apiUrl);
    requestUrl.setPath(searchPath);

    QMapIterator<QString, QString> searchList(*rowNameMap);
    while(searchList.hasNext()) {
        searchList.next();
        QString libId = searchList.key();
        QString text = searchList.value();
        text = text.replace(", ", " ");
        text = text.replace(",", " ");
        text = text.replace("(", "");
        text = text.replace(")", "");
        text = text.replace("[", "");
        text = text.replace("]", "");
        text = text.replace("-", "");

        QList<QPair<QString, QString> > queryItems = QList<QPair<QString, QString> >();
        queryItems << QPair<QString, QString>("query", text);
        queryItems << QPair<QString, QString>("facets[]", "fieldType:track");

        requestUrl.setQueryItems(queryItems);

        QNetworkRequest request(requestUrl);

        QNetworkReply *reply = manager->get(request);
        connect(reply, SIGNAL(finished()), textSearchMapper, SLOT(map()));
        textSearchMapper->setMapping(reply, libId);
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(getError(QNetworkReply::NetworkError)));
    }
    delete rowNameMap;
}

void SearchProvider::parseReplyForNameSearch(QString uid)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(static_cast<QSignalMapper *>(sender())->mapping(uid));

    QString jsonResponse(reply->readAll());
    QVariant response = QtJson::parse(jsonResponse);

    emit searchResultAvailable(uid, response.toMap()["results"].toList());

    reply->deleteLater();
}

void SearchProvider::downloadTrackPicture(const QString & picId)
{
    QNetworkRequest request(Constants::dynamicPictureUrl.arg(picId));

    QNetworkReply *reply = manager->get(request);
    downloadManagaer.insert(reply, picId);
    connect(reply, SIGNAL(readyRead()), this, SLOT(writeTrackPicture()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(getError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), this, SLOT(pictureDownloaded()));
}

void SearchProvider::writeTrackPicture()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());

    QString imgName = downloadManagaer.value(reply) + ".jpg";
    QFile imgFile(Constants::picturesLocation + QDir::separator() + imgName);

    imgFile.open(QIODevice::WriteOnly | QIODevice::Append);
    imgFile.write(reply->readAll());
    imgFile.close();
}

void SearchProvider::pictureDownloaded()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    emit pictureDownloadFinished(downloadManagaer.take(reply));
    reply->deleteLater();
}

void SearchProvider::getError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    qWarning() << QString(tr("Error on request %1 : %2"))
                  .arg(reply->request().url().toString())
                  .arg(error);
    reply->deleteLater();
}

