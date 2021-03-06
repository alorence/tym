/*****************************************************************************
Copyright 2012-2013 Antoine Lorence. All right reserved.

This file is part of TYM (Tag Your Music).

TYM (Tag Your Music) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TYM (Tag Your Music) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TYM (Tag Your Music). If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef SEARCHPROVIDER_H
#define SEARCHPROVIDER_H

#include <QObject>
#include <QSignalMapper>
#include <QNetworkReply>
#include <QJsonValue>

//NOTE: Missing doc on all methods

class SearchProvider : public QObject
{
    Q_OBJECT
public:
    explicit SearchProvider(QObject *parent = 0);
    ~SearchProvider();

signals:
    void searchResultAvailable(QString libId, QJsonValue result) const;

public slots:
    void beatportIdsBasedSearch(QMap<QString, QString> *);
    void naturalSearch(QMap<QString, QString> *);

private slots:
    void bpidSearchParseResponce();
    void naturalSearchParseResponse(QString);
    void requestError(QNetworkReply::NetworkError);

private :
    QNetworkAccessManager *_manager;

    QString _tracksPath;
    QString _searchPath;

    QMap<QNetworkReply *, QMap<QString, QString>*> _replyMap;
    QSignalMapper *_naturalSearchMapper;
};

#endif // SEARCHPROVIDER_H
