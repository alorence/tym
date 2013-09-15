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

#include <QUrlQuery>

#include <Logger.h>
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
    _naturalSearchMapper(new QSignalMapper(this))
{
    connect(_naturalSearchMapper, SIGNAL(mapped(QString)),
            this, SLOT(naturalSearchParseResponse(QString)));
}

SearchProvider::~SearchProvider()
{
    for(QNetworkReply* reply : _replyMap.keys())
       delete _replyMap.take(reply);

    delete _naturalSearchMapper;
    delete _manager;
}

void SearchProvider::beatportIdsBasedSearch(QMap<QString, QString> * uid_Bpid_Map)
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
    for (auto it = uid_Bpid_Map->begin() ; it != uid_Bpid_Map->end() ; ++it, ++i) {

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
        connect(reply, SIGNAL(finished()), this, SLOT(bpidSearchParseResponce()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(requestError(QNetworkReply::NetworkError)));

        LOG_DEBUG(QString("Request sent for IDs %1").arg(idsList));
        _replyMap.insert(reply, tempMap);
    }

    delete uid_Bpid_Map;
}

/* FIXME: Some requests never receive response
Need more tests. When searching automatically AND manually on the same track (same libId),
sometimes one of the responses does not call the slot. Therefore, it should because the requests
and associated QNetworkReply are totally different objects and should not be in conflict.
Maybe related to QNetworkSignalMapper. Also, what is the benefit of having a mapper here ? A simple
QMap<QReply, QString> should be sufficient
*/
void SearchProvider::naturalSearch(QMap<QString, QString> *id_SearchText_Map)
{
    O1Beatport * oauthManager = O1Beatport::instance();
    O1Requestor requestManager(_manager, oauthManager);
    if(!oauthManager->linked()) {
        LOG_ERROR("Unable to perform a search from text, Beatport OAuth is not linked");
        // TODO: emit an error signal
        return;
    }

    QUrl requestUrl(TYM_BEATPORT_API_URL + _searchPath);

    QMapIterator<QString, QString> searchList(*id_SearchText_Map);
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

        _naturalSearchMapper->setMapping(reply, libId);
        connect(reply, SIGNAL(finished()),
                _naturalSearchMapper, SLOT(map()));
        connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(requestError(QNetworkReply::NetworkError)));

        LOG_DEBUG(QString("Request sent for search \"%1\"").arg(text));
    }
    delete id_SearchText_Map;
}

void SearchProvider::bpidSearchParseResponce()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QMap<QString, QString> *uid_Bpid_Map = _replyMap.take(reply);

    QJsonDocument response = QJsonDocument::fromJson(reply->readAll());

    QJsonArray resultsArray = response.object()["results"].toArray();
    LOG_DEBUG(QString("Response received for id search: %1 results").arg(resultsArray.size()));

    QMapIterator<QString, QString> requestPair(*uid_Bpid_Map);
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

    delete uid_Bpid_Map;
    reply->deleteLater();
}

void SearchProvider::naturalSearchParseResponse(QString uid)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(static_cast<QSignalMapper *>(sender())->mapping(uid));

    QJsonDocument response = QJsonDocument::fromJson(reply->readAll());

    QJsonValue results = response.object()["results"];
    QString text = reply->request().url().query().split('&')[0].split('=')[1];
    LOG_DEBUG(QString("Response for \"%1\": %2 results").arg(text).arg(results.toArray().size()));
    emit searchResultAvailable(uid, results);

    reply->deleteLater();
}

void SearchProvider::requestError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    LOG_WARNING(QString("Error on request %1 : %2")
                .arg(reply->request().url().toString())
                .arg(error));
    reply->deleteLater();
}

