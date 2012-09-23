#include "searchprovider.h"

SearchProvider::SearchProvider(SettingsDialog *sd, QObject *parent) :
    QObject(parent),
    manager(new QNetworkAccessManager(this)),
    settings(sd)
{
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(receiveResponse(QNetworkReply*)));
    connect(manager, SIGNAL(networkSessionConnected()), this, SLOT(networkOk()));

    QNetworkConfiguration conf = manager->activeConfiguration();
    qDebug() << "active : " << conf.name();
    qDebug() << "active.state : " << conf.state();

    QNetworkConfigurationManager netConfManager;
    qDebug() << "default : " << netConfManager.defaultConfiguration().name();
    qDebug() << "state : " << netConfManager.defaultConfiguration().state();
    qDebug() << "online : " << netConfManager.isOnline();
    qDebug() << "capabilities : " << netConfManager.capabilities();

    manager->setConfiguration(netConfManager.defaultConfiguration());
    conf = manager->activeConfiguration();
    qDebug() << "active : " << conf.name();
    qDebug() << "active.state : " << conf.state();

}

void SearchProvider::networkOk(){
    qDebug() << "Network ok";
}

void SearchProvider::searchFromIds(QList<int> ids)
{
    /*
    QString h = settings.value("network/proxy/host").toString();
    qint16 p = settings.value("network/proxy/port").toInt();
    QString u = settings.value("network/proxy/user").toString();
    QString pwd = settings.value("network/proxy/pass").toString();
    QNetworkProxy proxy(QNetworkProxy::DefaultProxy, h, p, u, pwd);
    QNetworkProxy::setApplicationProxy(proxy);
    */

    manager->setNetworkAccessible(QNetworkAccessManager::Accessible );

    QString api = settings->getSettingsValue("settings/network/beatport/apihost").toString();
    QString args = QString("/catalog/3/tracks?ids=%1").arg(123456);

    QString urlPath = QString("http://%1%2").arg(api, args);
    QUrl url = QUrl(urlPath);
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(readyRead()), this, SLOT(parseJsonReply()));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(getError(QNetworkReply::NetworkError)));
    qDebug() << QString("Searching... on %1").arg(url.toString());
    qDebug() << manager->networkAccessible();
}

void SearchProvider::parseJsonReply()
{
    qDebug() << "ready !!";
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString jsonContent = QString(reply->readAll());

    qDebug() << "Content : \n" << jsonContent;
    QtJson::Json jsonParser;
    QVariant toto = jsonParser.parse(jsonContent);

    qDebug() << toto;

    reply->deleteLater();
}

void SearchProvider::getError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    qDebug() << "1 : " << reply->errorString();
    qDebug() << "2 : " << error;
    reply->deleteLater();
}

void SearchProvider::receiveResponse(QNetworkReply *reply)
{
    QString response = QString(reply->readAll());
    qDebug() << "Finished : " << response;

    reply->deleteLater();
}


