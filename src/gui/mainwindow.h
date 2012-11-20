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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>
#include <QtGui>

#include "ui_about.h"
#include "src/gui/settingsdialog.h"
#include "src/wizards/searchwizard.h"
#include "src/librarymodel.h"
#include "src/dbaccess/bpdatabase.h"
#include "src/network/searchprovider.h"
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
    void updateProgressBar();

    void on_actionDelete_triggered();

private:
    bool defaultConsoleDisplaying;

    Ui::MainWindow* ui;
    SettingsDialog* settings;
    SearchProvider searchProvider;
    BPDatabase databaseUtil;
    QSignalMapper * generalMapper;

    QWidget * console;
};

#endif // MAINWINDOW_H