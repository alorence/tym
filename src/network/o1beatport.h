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

#ifndef O1BEATPORT_H
#define O1BEATPORT_H

#include "o1.h"

#include <QDialog>

namespace Ui {
class BeatportAuthentication;
}

class O1Beatport : public O1
{
    Q_OBJECT
public:
    explicit O1Beatport(QObject *parent = 0);
    ~O1Beatport();

    void initAuthentication();
    void launchDialog();

private slots:
    /**
     * @brief Called when authentication needs to display the login page to the user
     * @param url
     */
    void onOpenBrowser(const QUrl &url);

    /**
     * @brief onLinkingSucceeded
     */
    void onLinkingSucceeded();

    /**
     * @brief onCloseBrowser
     */
    void onLinkingFailed();

private:

    Ui::BeatportAuthentication *_auth;
    QDialog * _dialog;
};

#endif // O1BEATPORT_H
