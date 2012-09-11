#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>
#include <QtGui>

#include "uis/settingsdialog.h"
#include "librarymodel.h"
#include "bpdatabase.h"
#include "searchprovider.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void importFilesToLibrary(QStringList);
    
private slots:
    void on_actionImport_triggered();

    void on_actionSettings_triggered();

    void on_actionSearch_triggered();

private:
    Ui::MainWindow* ui;
    SettingsDialog* settings;
    LibraryModel* libraryModel;
    QSqlRelationalTableModel* tracksModel;

};

#endif // MAINWINDOW_H
