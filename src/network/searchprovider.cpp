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

#include "searchprovider.h"

#include "commons.h"
#include "tools/utils.h"
#include "o1beatport.h"
#include "o1requestor.h"

SearchProvider::SearchProvider(QObject *parent) :
    QObject(parent),
    _manager(new QNetworkAccessManager(this)),
    _tracksPath("/catalog/3/tracks"),
    _searchPath("/catalog/3/search"),
    _replyMap(),
    _textSearchMapper(new QSignalMapper(this))
{
    connect(_textSearchMapper, SIGNAL(mapped(QString)), this, SLOT(parseReplyForNameSearch(QString)));
}

SearchProvider::~SearchProvider()
{
    for(QNetworkReply* reply : _replyMap.keys())
       delete _replyMap.take(reply);

    delete _textSearchMapper;
    delete _manager;
}

void SearchProvider::searchFromIds(QMap<QString, QString> * uidBpidMap)
{
    O1Beatport * oauthManager = O1Beatport::instance();
    O1Requestor requestManager(_manager, oauthManager);
    if(!oauthManager->linked()) {
        LOG_ERROR("Unable to perform a search from IDs, Beatport OAuth is not linked");
        // TODO: emit an error signal per request
        return;
    }

    QUrl requestUrl(TYM_BEATPORT_API_URL + _tracksPath);
    QList<QMap<QString, QString> *> splittedMaps;

    int i = 0;
    for (auto it = uidBpidMap->begin() ; it != uidBpidMap->end() ; ++it, ++i) {

        int listIndex = i / 10;
        if(splittedMaps.size() == listIndex) {
            splittedMaps.append(new QMap<QString, QString>());
        }

        splittedMaps.value(listIndex)->insert(it.key(), it.value());
    }

    for(auto tempMap : splittedMaps) {

        QString idsList = QStringList(tempMap->values()).join(",");

        QList<O1RequestParameter> params;
        params << O1RequestParameter("ids", idsList.toLatin1());

        requestUrl.setQuery(QUrlQuery(O1::createQueryParams(params)));
        QNetworkRequest request(requestUrl);

        QNetworkReply *reply = requestManager.get(request, params);
        connect(reply, SIGNAL(finished()), this, SLOT(parseReplyForIdSearch()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(requestError(QNetworkReply::NetworkError)));

        LOG_DEBUG(tr("Request sent for IDs %1").arg(idsList));
        _replyMap.insert(reply, tempMap);


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

        for(QJsonValue track : resultsArray) {
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
    O1Beatport * oauthManager = O1Beatport::instance();
    O1Requestor requestManager(_manager, oauthManager);
    if(!oauthManager->linked()) {
        LOG_ERROR("Unable to perform a search from IDs, Beatport OAuth is not linked");
        // TODO: emit an error signal per request
        return;
    }

    QUrl requestUrl(TYM_BEATPORT_API_URL + _searchPath);

    QMapIterator<QString, QString> searchList(*rowNameMap);
    while(searchList.hasNext()) {
        searchList.next();
        QString libId = searchList.key();
        QString text = searchList.value();

        if(text.isEmpty()) return;

        // Replace underscores by spaces (and prevent multiple spaces)
        Utils::instance()->simplifySpaces(text.replace('_', ' '));

        QList<O1RequestParameter> params;
        params << O1RequestParameter("query", text.toLatin1());
        params << O1RequestParameter("facets", "fieldType:track");

        requestUrl.setQuery(QUrlQuery(O1::createQueryParams(params)));
        QNetworkRequest request(requestUrl);

        QNetworkReply *reply = requestManager.get(request, params);

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

