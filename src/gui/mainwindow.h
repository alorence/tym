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
#include <QtWidgets>

#include "tools/searchprovider.h"

class SettingsDialog;
class PictureDownloader;
class BPDatabase;
class LibraryModel;
class SearchResultsModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void show();
    
private slots:
    void updateSearchResults(const QModelIndex & selected, const QModelIndex &);
    void updateTrackInfos(const QModelIndex, const QModelIndex);
    void updateLibraryActions();
    void updateSearchResultsActions();

    void on_libraryView_customContextMenuRequested(const QPoint &pos);

    void on_actionImport_triggered();
    void on_actionLibraryDelete_triggered();

    void on_searchResultsView_customContextMenuRequested(const QPoint &pos);
    void on_actionSetDefaultResult_triggered();
    void on_actionSearchResultDelete_triggered();

    void on_actionAbout_triggered();
    void on_actionSearch_triggered();
    void on_actionRename_triggered();

private:
    bool _defaultConsoleDisplaying;

    Ui::MainWindow* ui;
    SettingsDialog* _settings;
    PictureDownloader* _pictureDownloader;
    QSignalMapper* _generalMapper;

    BPDatabase* _dbHelper;
    LibraryModel* _libraryModel;
    SearchResultsModel* _searchModel;

    QThread* _libStatusUpdateThread;
};

#endif // MAINWINDOW_H
