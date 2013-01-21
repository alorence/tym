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

#include "commons.h"
#include "gui/settingsdialog.h"

SearchProvider::SearchProvider(QUrl bpApiHost, QObject *parent) :
    QObject(parent),
    _manager(new QNetworkAccessManager(this)),
    _apiUrl(bpApiHost),
    _replyMap(),
    _textSearchMapper(new QSignalMapper(this))
{
    _tracksPath = "/catalog/3/tracks";
    _searchPath = "/catalog/3/search";

    connect(_textSearchMapper, SIGNAL(mapped(QString)), this, SLOT(parseReplyForNameSearch(QString)));
}

SearchProvider::~SearchProvider()
{
    foreach(QNetworkReply* reply, _replyMap.keys())
       delete _replyMap.take(reply);

    delete _textSearchMapper;
    delete _manager;
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
    QUrl requestUrl(_apiUrl);
    requestUrl.setPath(_tracksPath);

    QUrlQuery query;
    query.addQueryItem("ids", QStringList(uidBpidMap->values()).join(","));
    requestUrl.setQuery(query);

    QNetworkRequest request(requestUrl);

    QNetworkReply *reply = _manager->get(request);

    _replyMap.insert(reply, uidBpidMap);

    connect(reply, SIGNAL(finished()), this, SLOT(parseReplyForIdSearch()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(requestError(QNetworkReply::NetworkError)));
}

void SearchProvider::parseReplyForIdSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QMap<QString, QString> *uidBpidMap = _replyMap.take(reply);

    QJsonDocument response = QJsonDocument::fromJson(reply->readAll());

    QMapIterator<QString, QString> req(*uidBpidMap);
    while(req.hasNext()) {
        req.next();

        QString uid = req.key();
        QString bpid = req.value();

        foreach(QJsonValue track, response.object()["results"].toArray()) {
            if(bpid == track.toObject().value("id").toVariant().toString()) {
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
    QUrl requestUrl(_apiUrl);
    requestUrl.setPath(_searchPath);

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

        QUrlQuery query;
        query.addQueryItem("query", text);
        query.addQueryItem("facets[]", "fieldType:track");

        requestUrl.setQuery(query);

        QNetworkRequest request(requestUrl);

        QNetworkReply *reply = _manager->get(request);
        connect(reply, SIGNAL(finished()), _textSearchMapper, SLOT(map()));
        _textSearchMapper->setMapping(reply, libId);
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(requestError(QNetworkReply::NetworkError)));
    }
    delete rowNameMap;
}

void SearchProvider::parseReplyForNameSearch(QString uid)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(static_cast<QSignalMapper *>(sender())->mapping(uid));

    QJsonDocument response = QJsonDocument::fromJson(reply->readAll());

    emit searchResultAvailable(uid, response.object()["results"]);

    reply->deleteLater();
}

void SearchProvider::requestError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    LOG_WARNING(tr("Error on request %1 : %2")
                .arg(reply->request().url().toString())
                .arg(error));
    reply->deleteLater();
}
