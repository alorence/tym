#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>
#include <QtGui>

#include "ui_about.h"
#include "uis/settingsdialog.h"
#include "uis/searchwizard.h"
#include "src/librarymodel.h"
#include "src/bpdatabase.h"
#include "src/searchprovider.h"
#include "src/patterntool.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void registerConsole(QWidget * c);

signals:
    void importFilesToLibrary(const QStringList) const;
    
private slots:
    void updateTrackInfos(QModelIndex,QModelIndex);
    void on_actionImport_triggered();
    void on_actionSearch_triggered();
    void on_actionAbout_triggered();

private:
    Ui::MainWindow* ui;
    SettingsDialog* settings;
    SearchProvider searchProvider;
    BPDatabase dbUtil;
    QSignalMapper * generalMapper;

    QWidget * console;
};

#endif // MAINWINDOW_H
