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

#include <QtGui/QApplication>
#include <QTextEdit>
#include "ui_mainwindow.h"
#include "uis/mainwindow.h"

QTextEdit * console;

void printConsoleMessage(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
    console->append(QString(msg));
    break;
    case QtWarningMsg:
    console->append(QString("Warning: %1").arg(msg));
    break;
    case QtCriticalMsg:
    console->append(QString("Critical: %1").arg(msg));
    break;
    case QtFatalMsg:
    console->append(QString("Fatal: %1").arg(msg));
    abort();
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    console = new QTextEdit();
    qInstallMsgHandler(printConsoleMessage);

    MainWindow w;

    w.registerConsole(console);

    a.setOrganizationName("Tag Your Music");
    a.setApplicationName("tym");
    a.setApplicationVersion("0.0.0.1");

    w.show();

    return a.exec();
}
