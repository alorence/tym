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
    void searchResultAvailable() const;
    
public slots:
    void searchFromIds(const QStringList ids);
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
};

#endif // SEARCHPROVIDER_H
