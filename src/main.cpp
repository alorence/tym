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


    a.setOrganizationName("Beatport Library Manager");
    a.setApplicationName("BPManager");
    a.setApplicationVersion("0.0.0.1");

    w.show();

    return a.exec();
}
