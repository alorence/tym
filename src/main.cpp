/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag Your Music) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag Your Music) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag Your Music).  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtWidgets/QApplication>
#include <QTextEdit>

#include <Logger.h>
#include <ConsoleAppender.h>

#include "ui_mainwindow.h"
#include "gui/mainwindow.h"
#include "commons.h"
#include "version.h"

QTextEdit * console;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ConsoleAppender* appender = new ConsoleAppender();
    appender->setFormat("[%-7l] <%c> %m\n");
#ifdef QT_DEBUG
    appender->setDetailsLevel(Logger::Trace);
#endif
    Logger::registerAppender(appender);
    console = new QTextEdit();

    a.setApplicationName("TagYourMusic");
    a.setApplicationDisplayName("Tag Your Music");
    a.setOrganizationDomain("tagyourmusic.net");
    a.setApplicationVersion(TYM_VERSION);

    {
        // Initialize some mandatory software items
        if( ! QDir(Constants::dataLocation()).exists()) {
            QDir().mkpath(Constants::dataLocation());
        }
        if( ! QDir(Constants::picturesLocation()).exists()) {
            QDir().mkpath(Constants::picturesLocation());
        }
    }


    qRegisterMetaType<QItemSelection>("QItemSelection");

    MainWindow w;
    w.registerConsole(console);
    w.show();

    return a.exec();
}
