#ifndef SEARCHPROVIDER_H
#define SEARCHPROVIDER_H

#include <QtCore>
#include <QtNetwork>

#include "libs/qt-json/json.h"
#include "uis/settingsdialog.h"

class SearchProvider : public QObject
{
    Q_OBJECT
public:
    explicit SearchProvider(SettingsDialog *sd, QObject *parent = 0);
    
signals:
    void searchResultAvailable(QMap<int, QVariant> resultMap) const;
    
public slots:
    void searchFromIds(QMap<int, QString> *idList);
    void searchFromName(const QStringList ids);

private slots:
    void parseJsonReply();
    void getError(QNetworkReply::NetworkError);

private :
    void initProxy();

    QNetworkAccessManager *manager;
    SettingsDialog* settings;
    QUrl searchUrl;
    QString tracksPath;

    QMap<QNetworkReply *, QMap<int, QString>*> replyMap;
};

#endif // SEARCHPROVIDER_H
