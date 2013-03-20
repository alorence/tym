/*****************************************************************************
Copyright 2013 Antoine Lorence. All right reserved.

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

#include "searchprovider.h"

#include "commons.h"
#include "tools/utils.h"
#include "gui/settingsdialog.h"

SearchProvider::SearchProvider(QObject *parent) :
    QObject(parent),
    _manager(new QNetworkAccessManager(this)),
    _replyMap(),
    _textSearchMapper(new QSignalMapper(this))
{
    _tracksPath = "/catalog/3/tracks";
    _searchPath = "/catalog/3/search";

    connect(_textSearchMapper, SIGNAL(mapped(QString)), this, SLOT(parseReplyForNameSearch(QString)));

    QSettings settings;
    _apiUrl = settings.value(TYM_PATH_API_URL, TYM_DEFAULT_API_URL).toString();
}

SearchProvider::~SearchProvider()
{
    foreach(QNetworkReply* reply, _replyMap.keys())
       delete _replyMap.take(reply);

    delete _textSearchMapper;
    delete _manager;
}

void SearchProvider::searchFromIds(QMap<QString, QString> * uidBpidMap)
{
    QUrl requestUrl(_apiUrl);
    requestUrl.setPath(_tracksPath);

    QList<QMap<QString, QString> *> splittedMaps;
    int i = 0;
    for (auto it = uidBpidMap->begin() ; it != uidBpidMap->end() ; ++it, ++i) {

        int listIndex = i / 10;
        if(splittedMaps.size() == listIndex) {
            splittedMaps.append(new QMap<QString, QString>());
        }

        splittedMaps.value(listIndex)->insert(it.key(), it.value());
    }

    foreach(auto tempMap, splittedMaps) {
        QUrlQuery query;
        QString idsList = QStringList(tempMap->values()).join(",");
        query.addQueryItem("ids", idsList);
        requestUrl.setQuery(query);

        QNetworkRequest request(requestUrl);

        QNetworkReply *reply = _manager->get(request);

        LOG_DEBUG(tr("Request sent for IDs %1").arg(idsList));

        _replyMap.insert(reply, tempMap);

        connect(reply, SIGNAL(finished()), this, SLOT(parseReplyForIdSearch()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(requestError(QNetworkReply::NetworkError)));

        tempMap = new QMap<QString, QString>();
    }

    delete uidBpidMap;
}

void SearchProvider::parseReplyForIdSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QMap<QString, QString> *uidBpidMap = _replyMap.take(reply);

    QJsonDocument response = QJsonDocument::fromJson(reply->readAll());

    QJsonArray resultsArray = response.object()["results"].toArray();
    LOG_DEBUG(tr("Response received for id search: %1 results").arg(resultsArray.size()));

    QMapIterator<QString, QString> requestPair(*uidBpidMap);
    while(requestPair.hasNext()) {
        requestPair.next();

        QString uid = requestPair.key();
        QString bpid = requestPair.value();

        foreach(QJsonValue track, resultsArray) {
            if(bpid == track.toObject().value("id").toVariant().toString()) {
                emit searchResultAvailable(uid, track);
                break;
            }
        }
    }

    delete uidBpidMap;
    reply->deleteLater();
}


void SearchProvider::searchManually(QMap<QString, QString> *rowNameMap)
{
    QUrl requestUrl(_apiUrl);
    requestUrl.setPath(_searchPath);

    QMapIterator<QString, QString> searchList(*rowNameMap);
    while(searchList.hasNext()) {
        searchList.next();
        QString libId = searchList.key();
        QString text = searchList.value();

        // Replace underscores by spaces (and prevent multiple spaces)
        Utils::simplifySpaces(text.replace('_', ' '));

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

        LOG_DEBUG(tr("Send request: %1").arg(request.url().toString()));
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

