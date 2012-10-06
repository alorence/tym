#include "searchprovider.h"

SearchProvider::SearchProvider(SettingsDialog *sd, QObject *parent) :
    QObject(parent),
    manager(new QNetworkAccessManager(this)),
    settings(sd),
    replyMap()
{
    searchUrl = QUrl(QString("http://%1").arg(settings->getSettingsValue("settings/network/beatport/apihost").toString()));
    tracksPath = "/catalog/3/tracks";
}

void SearchProvider::searchFromIds(QMap<int, QString> * idList)
{
    QUrl requestUrl = QUrl(searchUrl);
    requestUrl.setPath(tracksPath);

    QList<QPair<QString, QString> > queryItems = QList<QPair<QString, QString> >();
    queryItems << QPair<QString, QString>("ids", QStringList(idList->values()).join(","));
    requestUrl.setQueryItems(queryItems);

    QNetworkRequest request(requestUrl);

    QNetworkReply *reply = manager->get(request);

    replyMap.insert(reply, idList);

    connect(reply, SIGNAL(finished()), this, SLOT(parseJsonReply()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(getError(QNetworkReply::NetworkError)));
}

void SearchProvider::searchFromName(const QStringList ids)
{
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

void SearchProvider::parseJsonReply()
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

void SearchProvider::getError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    qWarning() << QString(tr("Request %1 : %2"))
                  .arg(reply->request().url().toString())
                  .arg(error);
    reply->deleteLater();
}

