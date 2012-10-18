#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>
#include <QtGui>

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

signals:
    void importFilesToLibrary(const QStringList) const;
    
private slots:
    void on_actionImport_triggered();
    void on_actionSearch_triggered();
    void updateTrackInfos(QModelIndex,QModelIndex);

private:
    Ui::MainWindow* ui;
    SettingsDialog* settings;
    SearchProvider searchProvider;
    BPDatabase dbUtil;
    QSignalMapper * generalMapper;
};

#endif // MAINWINDOW_H
