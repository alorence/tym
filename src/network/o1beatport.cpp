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

#include <QDesktopServices>

#include <Logger.h>
#include "config.h"

O1Beatport::O1Beatport(QObject *parent) :
    O1(parent),
    _dialogContent(new Ui::BeatportAuthentication),
    _dialog(new QDialog),
    _status(InitialState)
{
    if(QByteArray(BEATPORT_API_KEY).isEmpty() || QByteArray(BEATPORT_API_SECRET).isEmpty()) {
        _status = APIKeysMissing;
        return;
    } else {
        setClientId(BEATPORT_API_KEY);
        setClientSecret(BEATPORT_API_SECRET);
    }

    _dialogContent->setupUi(_dialog);
    connect(_dialogContent->webView, &QWebView::linkClicked,
            this, &O1Beatport::onLinkClickedInsideWebView);

    setAccessTokenUrl(QUrl("https://oauth-api.beatport.com/identity/1/oauth/access-token"));
    setAuthorizeUrl(QUrl("https://oauth-api.beatport.com/identity/1/oauth/authorize"));
    setRequestTokenUrl(QUrl("https://oauth-api.beatport.com/identity/1/oauth/request-token"));

    connect(this, &O1::openBrowser, this, &O1Beatport::onOpenBrowser);
    connect(this, &O1::closeBrowser, _dialog, &QDialog::hide);
    connect(this, &O1::linkedChanged, this, &O1Beatport::onLinkingChanged);
    connect(this, &O1::linkingSucceeded, this, &O1Beatport::onLinkingSucceeded);
}

O1Beatport::~O1Beatport()
{
    _dialogContent->webView->page()->deleteLater();
    _dialogContent->webView->deleteLater();
    _dialog->deleteLater();
    delete _dialogContent;
}

void O1Beatport::link()
{
    if(_status == APIKeysMissing) {
        emit statusChanged(_status);
        LOG_ERROR("Beatport API keys are missing. Check the configuration for ");
        LOG_ERROR("CMake variables BEATPORT_API_KEY and BEATPORT_API_SECRET. You ");
        LOG_ERROR("can request keys at https://accounts.beatport.com/developer/request-api-key");
    } else {
        O1::link();
    }
}

void O1Beatport::onOpenBrowser(const QUrl &url)
{
    _dialogContent->webView->load(url);
    _dialogContent->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    _dialog->show();
}

void O1Beatport::onLinkClickedInsideWebView(const QUrl &url)
{
    QDesktopServices::openUrl(url);
}

void O1Beatport::onLinkingSucceeded()
{
    // This slot is used both at the end of link() and unlink() methods
    if(linked() && _status != Linked) {
        _status = Linked;
    } else if(!linked() && _status != Notlinked) {
        _status = Notlinked;
    } else {
        return;
    }
    emit statusChanged(_status);
}

void O1Beatport::onLinkingChanged()
{
    if(linked()) {
        _status = Linked;
    } else {
        _status = Notlinked;
    }
    emit statusChanged(_status);
}
