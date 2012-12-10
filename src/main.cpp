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

#include <QtGui/QApplication>
#include <QTextEdit>
#include "ui_mainwindow.h"
#include "src/gui/mainwindow.h"
#include "commons.h"

QTextEdit * console;

void printConsoleMessage(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
    console->append(msg);
    break;
    case QtWarningMsg:
    console->append(QObject::tr("Warning: %1").arg(msg));
    break;
    case QtCriticalMsg:
    console->append(QObject::tr("Critical: %1").arg(msg));
    break;
    case QtFatalMsg:
    console->append(QObject::tr("Fatal: %1").arg(msg));
    abort();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("Tag Your Music");
    a.setApplicationName("tym");
    a.setApplicationVersion("0.0.0.1");

    Constants::dataLocation = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    Constants::picturesLocation = Constants::dataLocation + QDir::separator() + "albumarts";

    {
        // Initialize some mandatory software items
        if( ! QDir(Constants::dataLocation).exists()) {
            QDir().mkpath(Constants::dataLocation);
        }
        if( ! QDir(Constants::picturesLocation).exists()) {
            QDir().mkpath(Constants::picturesLocation);
        }
    }

    console = new QTextEdit();
    qInstallMsgHandler(printConsoleMessage);

    qRegisterMetaType<QItemSelection>("QItemSelection");

    MainWindow w;
    w.registerConsole(console);
    w.show();

    return a.exec();
}
