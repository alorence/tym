#include "searchprovider.h"

SearchProvider::SearchProvider(SettingsDialog *sd, QObject *parent) :
    QObject(parent),
    manager(new QNetworkAccessManager(this)),
    settings(sd)
{
    searchUrl = QUrl(QString("http://%1").arg(settings->getSettingsValue("settings/network/beatport/apihost").toString()));
    tracksPath = "/catalog/3/tracks";
}

void SearchProvider::searchFromIds(const QStringList ids)
{
    QUrl requestUrl = QUrl(searchUrl);
    requestUrl.setPath(tracksPath);

    QList<QPair<QString, QString> > queryItems = QList<QPair<QString, QString> >();
    queryItems << QPair<QString, QString>("ids", ids.join(","));
    requestUrl.setQueryItems(queryItems);

    QNetworkRequest request(requestUrl);

    QNetworkReply *reply = manager->get(request);

    connect(reply, SIGNAL(readyRead()), this, SLOT(parseJsonReply()));
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

    QtJson::Json jsonParser;
    QVariant response = jsonParser.parse(reply->readAll());

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

