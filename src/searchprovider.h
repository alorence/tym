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
    ~SearchProvider();
    
signals:
    void searchResultAvailable(int row, QVariant result) const;
    
public slots:
    void searchFromIds(QMap<int, QString> *);
    void searchFromName(QMap<int, QString> *);

private slots:
    void parseReplyForIdSearch();
    void parseReplyForNameSearch(int row);
    void getError(QNetworkReply::NetworkError);

private :
    void initProxy();

    QNetworkAccessManager *manager;
    SettingsDialog* settings;
    QUrl apiUrl;
    QString tracksPath;
    QString searchPath;

    QMap<QNetworkReply *, QMap<int, QString>*> replyMap;
    QSignalMapper *textSearchMapper;
};

#endif // SEARCHPROVIDER_H
