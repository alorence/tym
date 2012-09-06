#ifndef SEARCHPROVIDER_H
#define SEARCHPROVIDER_H

#include <QObject>

class SearchProvider : public QObject
{
    Q_OBJECT
public:
    explicit SearchProvider(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // SEARCHPROVIDER_H
