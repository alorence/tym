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

    a.setOrganizationName("Tag Your Music");
    a.setApplicationName("tym");
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

    ConsoleAppender* consoleAppender = new ConsoleAppender();
    consoleAppender->setFormat("[%-7l] <%C> %m\n");
    Logger::registerAppender(consoleAppender);

    console = new QTextEdit();

    qRegisterMetaType<QItemSelection>("QItemSelection");

    MainWindow w;
    w.registerConsole(console);
    w.show();

    return a.exec();
}
