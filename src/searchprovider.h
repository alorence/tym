/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag your Library) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag your Library) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag your Library).  If not, see <http://www.gnu.org/licenses/>.
*/

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
    void pictureDownloadFinished(QString picId, QString picLocalPath) const;

public slots:
    void searchFromIds(QMap<int, QString> *);
    void searchFromName(QMap<int, QString> *);
    void downloadTrackPicture(const QString & picId);

private slots:
    void parseReplyForIdSearch();
    void parseReplyForNameSearch(int row);
    void writeTrackPicture(QString trackId);
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
    QSignalMapper *pictureDownloadMapper;
};

#endif // SEARCHPROVIDER_H
