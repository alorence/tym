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

#include "searchprovider.h"

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

    connect(textSearchMapper, SIGNAL(mapped(int)), this, SLOT(parseReplyForNameSearch(int)));
}

SearchProvider::~SearchProvider()
{
    foreach(QNetworkReply* reply, replyMap.keys())
       delete replyMap.take(reply);
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

void SearchProvider::searchFromIds(QMap<int, QString> * rowBpidMap)
{
    QUrl requestUrl = QUrl(apiUrl);
    requestUrl.setPath(tracksPath);

    QList<QPair<QString, QString> > queryItems = QList<QPair<QString, QString> >();
    queryItems << QPair<QString, QString>("ids", QStringList(rowBpidMap->values()).join(","));
    requestUrl.setQueryItems(queryItems);

    QNetworkRequest request(requestUrl);

    QNetworkReply *reply = manager->get(request);

    replyMap.insert(reply, rowBpidMap);

    connect(reply, SIGNAL(finished()), this, SLOT(parseReplyForIdSearch()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(getError(QNetworkReply::NetworkError)));
}
void SearchProvider::parseReplyForIdSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QMap<int, QString> *requestMap = replyMap.take(reply);

    QString jsonResponse(reply->readAll());
    QVariant response = QtJson::Json::parse(jsonResponse);

    QVariant track;
    QMapIterator<int, QString> req(*requestMap);

    while(req.hasNext()) {
        req.next();

        int id = req.key();
        QString bpid = req.value();

        foreach(track, response.toMap()["results"].toList()) {
            if(bpid == track.toMap()["id"].toString()) {
                emit searchResultAvailable(id, track);
                break;
            }
        }
    }

    // TODO : Insert error code for tracks not found

    delete requestMap;
    reply->deleteLater();
}


void SearchProvider::searchFromName(QMap<int, QString> *nameList)
{
    QUrl requestUrl = QUrl(apiUrl);
    requestUrl.setPath(searchPath);


    QMapIterator<int, QString> searchList(*nameList);
    while(searchList.hasNext()) {
        searchList.next();
        int index = searchList.key();
        QString text = searchList.value();
        text = text.replace(", ", " ");
        text = text.replace(",", " ");
        text = text.replace("(", "");
        text = text.replace(")", "");
        text = text.replace("[", "");
        text = text.replace("]", "");

        QList<QPair<QString, QString> > queryItems = QList<QPair<QString, QString> >();
        queryItems << QPair<QString, QString>("query", text);
        queryItems << QPair<QString, QString>("facets[]", "fieldType:track");

        requestUrl.setQueryItems(queryItems);

        QNetworkRequest request(requestUrl);

        QNetworkReply *reply = manager->get(request);
        connect(reply, SIGNAL(finished()), textSearchMapper, SLOT(map()));
        textSearchMapper->setMapping(reply, index);
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(getError(QNetworkReply::NetworkError)));
    }
    delete nameList;
}
void SearchProvider::parseReplyForNameSearch(int row)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(static_cast<QSignalMapper *>(sender())->mapping(row));

    QString jsonResponse(reply->readAll());
    QVariant response = QtJson::Json::parse(jsonResponse);

    emit searchResultAvailable(row, response.toMap()["results"].toList());

    // TODO : Insert error code for tracks not found

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

