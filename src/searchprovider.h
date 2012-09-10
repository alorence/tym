#ifndef SEARCHPROVIDER_H
#define SEARCHPROVIDER_H

#include <QtCore>
#include <QtNetwork>

#include "libs/qt-json/json.h"

class SearchProvider : public QObject
{
    Q_OBJECT
public:
    explicit SearchProvider(QObject *parent = 0);
    
signals:
    
public slots:
    void searchFromIds(QList<int> ids);

private slots:
    void parseJsonReply();
    void getError(QNetworkReply::NetworkError);

private :
    QNetworkAccessManager *manager;
    
};

#endif // SEARCHPROVIDER_H
