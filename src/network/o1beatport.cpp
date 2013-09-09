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

#include "o1beatport.h"
#include "ui_o1beatport.h"

O1Beatport::O1Beatport(QObject *parent) :
    O1(parent),
    _auth(new Ui::BeatportAuthentication),
    _dialog(new QDialog)
{
    _auth->setupUi(_dialog);

//    connect(this, &O1::linkedChanged, this, &O1Beatport::onLinkedChanged);
    connect(this, &O1::linkingSucceeded, this, &O1Beatport::onLinkingSucceeded);
    connect(this, &O1::linkingFailed, this, &O1Beatport::onLinkingFailed);
    connect(this, &O1::openBrowser, this, &O1Beatport::onOpenBrowser);

    connect(_auth->startButton, &QPushButton::clicked, this, &O1Beatport::initAuthentication);
}

O1Beatport::~O1Beatport()
{
    _dialog->deleteLater();
    delete _auth;
}

void O1Beatport::initAuthentication()

{
    setAccessTokenUrl(QUrl("https://oauth-api.beatport.com/identity/1/oauth/access-token"));
    setAuthorizeUrl(QUrl("https://oauth-api.beatport.com/identity/1/oauth/authorize"));
    setRequestTokenUrl(QUrl("https://oauth-api.beatport.com/identity/1/oauth/request-token"));

    link();
}

void O1Beatport::launchDialog()
{
    _auth->pages->setCurrentWidget(_auth->startPage);
    _dialog->exec();
}

void O1Beatport::onOpenBrowser(const QUrl &url)
{
    _auth->webView->load(url);
    _auth->pages->setCurrentWidget(_auth->loginPage);
}

void O1Beatport::onLinkingSucceeded()
{
    _auth->pages->setCurrentWidget(_auth->successPage);
}

void O1Beatport::onLinkingFailed()
{
    _auth->pages->setCurrentWidget(_auth->errorPage);
}
