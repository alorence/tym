#include "searchprovider.h"

SearchProvider::SearchProvider(QObject *parent) :
    QObject(parent),
    manager(new QNetworkAccessManager(this))
{
}

void SearchProvider::searchFromIds(QList<int> ids)
{
    QSettings settings;
    QString h = settings.value("network/proxy/host").toString();
    qint16 p = settings.value("network/proxy/port").toInt();
    QString u = settings.value("network/proxy/user").toString();
    QString pwd = settings.value("network/proxy/pass").toString();
    QNetworkProxy proxy(QNetworkProxy::DefaultProxy, h, p, u, pwd);
    QNetworkProxy::setApplicationProxy(proxy);

    QString api = settings.value("network/beatport/host").toString();
    QString args = QString("/catalog/3/tracks?ids=%1").arg(123456);

    QNetworkRequest request(QUrl(QString("http://%1%2").arg(api, args)));
    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(readyRead()), this, SLOT(parseJsonReply()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(getError(QNetworkReply::NetworkError)));
}

void SearchProvider::parseJsonReply()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString jsonContent = QString(reply->readAll());

    QtJson::Json jsonParser;
    QVariant toto = jsonParser.parse(jsonContent);

    qDebug() << toto;

    reply->deleteLater();
}

void SearchProvider::getError(QNetworkReply::NetworkError)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    reply->deleteLater();
}


