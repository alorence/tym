/*****************************************************************************
Copyright 2013 Antoine Lorence. All right reserved.

This file is part of TYM (Tag Your Music).

TYM (Tag Your Music) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TYM (Tag Your Music) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TYM (Tag Your Music). If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtSql>
#include <QtWidgets>

#include "tools/searchprovider.h"
#include "dbaccess/librarymodel.h"

class About;
class SettingsDialog;
class PictureDownloader;
class BPDatabase;
class SearchResultsModel;

namespace Ui {
class MainWindow;
}
/*!
 * \brief Define the main application window
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    /*!
     * \brief Build the main window.
     * Initialize all widgets and classes it will use during the execution. Connect all
     * signals / slots used, launch a LibraryStatusUpdater in a QThread.
     * \param parent
     * \sa LibraryStatusUpdater
     * \sa QThread
     * \sa LibraryModel
     * \sa SearchResultsModel
     * \sa TrackInfosView
     */
    explicit MainWindow(QWidget *parent = 0);
    /*!
     * \brief Destruct the windows and all children which need.
     */
    ~MainWindow();

    /*!
     * \brief Define if elements dragged to this windows can or not be accepted.
     * \sa QWidget::dragEnterEvent()
     */
    void dragEnterEvent(QDragEnterEvent *);
    /*!
     * \brief Launch the right action when user drop elements to this window.
     * \sa QWidget::dropEvent()
     */
    void dropEvent(QDropEvent *);
    /*!
     * \brief Called when windows is displayed.
     * Reimplemented from QMainWindow to configure some little things in the views. Some options need to be set
     * while the window has start to be rendered. For example, configuring the size of QTableView column using
     * the total with of the widget can be done only in this function, because QHeaderView::width() has a valid
     * result only here.
     */
    void showEvent(QShowEvent *);

private slots:
    /*!
     * \brief Show or hide console in window
     */
    void toggleConsoleDisplaying(bool show) const;

    /*!
     * \brief Update the general settings for the application.
     */
    void updateSettings();
    /*!
     * \brief Update results list in the search results view.
     * Called when user change the item selected in the library view. Connected to the
     * library view's selectionModel(), when it emits its currentRowChanged() signal.
     * \param selected the new selected element in library view
     * \param deselected the unselected element in library view
     * \sa QItemSelectionModel::currentRowChanged()
     */
    void updateSearchResults(const QModelIndex &selected, const QModelIndex &deselected);
    /*!
     * \brief Update informations displayed in the TrackInfosView widget.
     * Called when search results view's selectionModel() emit its currentRowChanged() signal.
     * \param selected the new selected elements in search result view
     * \param deselected the unselected elements in search result view
     * \sa QItemSelectionModel::currentRowChanged()
     */
    void updateTrackInfos(const QModelIndex &selected, const QModelIndex &deselected);
    /*!
     * \brief Enable or disable actions according to number of library elements currently selected
     */
    void updateLibraryActions(int numSelected = -1);
    /*!
     * \brief Enable or disable actions according to number of search results displayed an,d selected
     */
    void updateSearchResultsActions();

    /*!
     * \brief Select library elements.
     *
     * \param comboIndex index of the combo element selected
     */
    void selectSpecificLibraryElements(int comboIndex);

    /*!
     * \brief Open the context menu on library table view
     * \param pos Global mouse position
     */
    void on_libraryView_customContextMenuRequested(const QPoint &pos);

    /*!
     * \brief Open the import dialog and import files in database
     */
    void on_actionImport_triggered();
    /*!
     * \brief Delete a library entry in database
     */
    void on_actionRemove_triggered();

    /*!
     * \brief Open the context menu on search result table view
     * \param pos Global mouse position
     */
    void on_searchResultsView_customContextMenuRequested(const QPoint &pos);
    /*!
     * \brief Link a result to a track in library
     */
    void on_actionSetDefaultResult_triggered();
    /*!
     * \brief Delete a selected search result from the database
     */
    void on_actionSearchResultDelete_triggered();

    /*!
     * \brief Open the search wizard
     */
    void on_actionSearch_triggered();
    /*!
     * \brief Open the rename wizard
     */
    void on_actionRename_triggered();

    /*!
     * \brief Open the export playlist wizard
     */
    void on_actionExport_triggered();

private:
    /*!
     * \brief Return a list of files supported by the application
     * If \a entry represents a directory, return a list of files inside it and all its subirectories.
     * If \a represents a file, return the QFileInfo encapsulated in a QList.
     *
     * In both cases, resulting list contains only files supported by the application.
     * \param A QFileInfo representing a file or a directory
     * \return List of QFileInfo pointing to files supported by this application
     */
    const QFileInfoList filteredFileList(const QFileInfo &entry) const;

    /*!
     * \brief Set to true to show console by default
     */
    bool _defaultConsoleDisplaying;

    Ui::MainWindow* ui;
    About * _aboutDialog;
    SettingsDialog* _settings;
    PictureDownloader* _pictureDownloader;

    BPDatabase* _dbHelper;
    LibraryModel* _libraryModel;
    SearchResultsModel* _searchModel;

    QThread* _libStatusUpdateThread;

//    QMap<LibraryModel::GroupSelection,QString> _selectionActions;
    QSignalMapper _selectionMapper;
    QList<QAction*> _selectActionsList;
};

#endif // MAINWINDOW_H
