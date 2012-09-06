#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setOrganizationName("Beatport Library Manager");
    a.setApplicationName("BPManager");
    a.setApplicationVersion("0.0.0.1");

    MainWindow w;
    w.show();

    return a.exec();
}
