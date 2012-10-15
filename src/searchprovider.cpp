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

void SearchProvider::searchFromIds(QMap<int, QString> * idList)
{
    QUrl requestUrl = QUrl(apiUrl);
    requestUrl.setPath(tracksPath);

    QList<QPair<QString, QString> > queryItems = QList<QPair<QString, QString> >();
    queryItems << QPair<QString, QString>("ids", QStringList(idList->values()).join(","));
    requestUrl.setQueryItems(queryItems);

    QNetworkRequest request(requestUrl);

    QNetworkReply *reply = manager->get(request);

    replyMap.insert(reply, idList);

    connect(reply, SIGNAL(finished()), this, SLOT(parseReplyForIdSearch()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(getError(QNetworkReply::NetworkError)));
}

void SearchProvider::searchFromName(QMap<int, QString> *nameList)
{
    QUrl requestUrl = QUrl(apiUrl);
    requestUrl.setPath(tracksPath);


    QMapIterator<int, QString> searchList(*nameList);
    while(searchList.hasNext()) {
        searchList.next();
        int index = searchList.key();
        QString text = searchList.value();

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

void SearchProvider::parseReplyForIdSearch()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QMap<int, QString> *requestMap = replyMap.take(reply);

    QString jsonResponse(reply->readAll());
    QVariant response = QtJson::Json::parse(jsonResponse);

    QMap<int, QVariant> indexedResults;
    QVariant track;
    QMapIterator<int, QString> req(*requestMap);
    while(req.hasNext()) {
        req.next();
        int id = req.key();
        QString bpid = req.value();
        foreach(track, response.toMap()["results"].toList()) {
            if(bpid == track.toMap()["id"].toString()) {
                indexedResults[id] = track;
                break;
            }
        }
    }

    emit searchResultAvailable(indexedResults);

    // TODO : Insert error code for tracks not found

    delete(requestMap);
    reply->deleteLater();
}

void SearchProvider::parseReplyForNameSearch(int index)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(static_cast<QSignalMapper *>(sender())->mapping(index));

    QString jsonResponse(reply->readAll());
    QVariant response = QtJson::Json::parse(jsonResponse);


    QMap<int, QVariant> indexedResults;
    indexedResults[index] = response.toMap()["results"].toList();
    emit searchResultAvailable(indexedResults);

    // TODO : Insert error code for tracks not found

    reply->deleteLater();
}

void SearchProvider::getError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    qWarning() << QString(tr("Request %1 : %2"))
                  .arg(reply->request().url().toString())
                  .arg(error);
    reply->deleteLater();
}

