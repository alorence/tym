#ifndef SEARCHPROVIDER_H
#define SEARCHPROVIDER_H

#include <QtCore>
#include <QtNetwork>

class SearchProvider : public QObject
{
    Q_OBJECT
public:
    explicit SearchProvider(QObject *parent = 0);
    
signals:
    
public slots:
    void searchFromIds(QList<int> ids);

private:
    QHttp* httpSocket;
    
};

#endif // SEARCHPROVIDER_H
