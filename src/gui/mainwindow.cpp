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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <Logger.h>
#include <WidgetAppender.h>

#include "ui_about.h"
#include "settingsdialog.h"
#include "dbaccess/librarymodel.h"
#include "dbaccess/searchresultsmodel.h"
#include "dbaccess/bpdatabase.h"
#include "wizards/searchwizard.h"
#include "wizards/renamewizard.h"
#include "tools/patterntool.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    defaultConsoleDisplaying(false),
    ui(new Ui::MainWindow),
    settings(new SettingsDialog(this)),
    searchProvider(QUrl(QString("http://%1").arg(settings->getSettingsValue("settings/network/beatport/apihost").toString())), this),
    _dbHelper(new BPDatabase)
{
    ui->setupUi(this);
    connect(ui->actionSettings, SIGNAL(triggered()), settings, SLOT(open()));

    // Configure console message displaying
    connect(ui->actionToggleConsole, SIGNAL(toggled(bool)), ui->outputConsole, SLOT(setVisible(bool)));
    ui->actionToggleConsole->setChecked(defaultConsoleDisplaying);
    ui->outputConsole->setVisible(defaultConsoleDisplaying);

    WidgetAppender* widgetAppender = new WidgetAppender(ui->outputConsole);
    widgetAppender->setFormat("%m\n");
    Logger::registerAppender(widgetAppender);

    if( ! _dbHelper->initialized()) {
        LOG_ERROR(tr("Impossible to connect with database..."));
        return;
    }

    _libraryModel = new LibraryModel(this, _dbHelper->dbObject());
    _libraryModel->setTable("LibraryHelper");
    _libraryModel->select();

    _searchModel = new SearchResultsModel(this, _dbHelper->dbObject());
    _searchModel->setTable("SearchResultsHelper");
    _searchModel->select();

    // Used to transfer fixed parameters to some slots
    generalMapper = new QSignalMapper(this);

    /**
     * Library
     */
    // Configure view
    ui->libraryView->setModel(_libraryModel);
    ui->libraryView->hideColumn(Library::Uid);
    ui->libraryView->hideColumn(Library::Bpid);

    // Check rows in model when selection change on the view
    connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
            _libraryModel, SLOT(updateCheckedRows(const QItemSelection&,const QItemSelection&)));
    // Select or deselect rows on the view when checkboxes are checked / unchecked
    connect(_libraryModel, SIGNAL(rowCheckedOrUnchecked(QItemSelection,QItemSelectionModel::SelectionFlags)),
            ui->libraryView->selectionModel(), SLOT(select(QItemSelection,QItemSelectionModel::SelectionFlags)));
    // Set current selection index to last modified row
    connect(_libraryModel, SIGNAL(rowCheckedOrUnchecked(QModelIndex,QItemSelectionModel::SelectionFlags)),
            ui->libraryView->selectionModel(), SLOT(setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)));
    // Update search results view when selecting something in the library view
    connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateSearchResults(const QModelIndex&,const QModelIndex&)));

    connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
            this, SLOT(updateLibraryActions()));

    /**
     * Search Results View
     */
    // Configure view
    ui->searchResultsView->setModel(_searchModel);
    ui->searchResultsView->hideColumn(SearchResults::LibId);
    ui->searchResultsView->hideColumn(SearchResults::Bpid);
    ui->searchResultsView->hideColumn(SearchResults::DefaultFor);

    // When the selection change in library view, the search results view should be reconfigured.
    connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            generalMapper, SLOT(map()));
    // The first row (0) must be selected :
    generalMapper->setMapping(ui->libraryView->selectionModel(), 0);
    connect(generalMapper, SIGNAL(mapped(int)), ui->searchResultsView, SLOT(selectRow(int)));

    // Display informations about a track when selecting it in the view
    connect(ui->searchResultsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateTrackInfos(QModelIndex,QModelIndex)));
    connect(ui->trackInfos, SIGNAL(downloadPicture(const QString&)),
            &searchProvider, SLOT(downloadTrackPicture(const QString&)));
    connect(&searchProvider, SIGNAL(pictureDownloadFinished(QString)),
            ui->trackInfos, SLOT(displayDownloadedPicture(QString)));
    connect(_dbHelper, SIGNAL(referenceForTrackUpdated(QString)),
            _searchModel, SLOT(refresh(QString)));

    connect(ui->searchResultsView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
            this, SLOT(updateSearchResultsActions()));


    /**
     * Actions
     */
    connect(&searchProvider, SIGNAL(searchResultAvailable(QString,QJsonValue)),
            _dbHelper, SLOT(storeSearchResults(QString,QJsonValue)));
    connect(_dbHelper, SIGNAL(libraryEntryUpdated(QString)),
            _libraryModel, SLOT(refresh()));

    updateLibraryActions();
    updateSearchResultsActions();
}

MainWindow::~MainWindow()
{
    _dbHelper->deleteLater();
    delete ui;
    delete generalMapper;
}

void MainWindow::show()
{
    QMainWindow::show();

    // Configure libraryView filePath column to take as space as possible
    QHeaderView *horizHeader = ui->libraryView->horizontalHeader();
    ui->libraryView->setColumnWidth(Library::FilePath, horizHeader->width() - 2 * horizHeader->defaultSectionSize());
}

void MainWindow::updateSearchResults(const QModelIndex & selected, const QModelIndex &)
{
    QVariant libId = _libraryModel->data(_libraryModel->index(selected.row(), Library::Uid));
    _searchModel->setFilter("libId=" + libId.toString());

    if( ! _searchModel->query().isActive()) {
        _searchModel->select();
    }
}


void MainWindow::updateTrackInfos(const QModelIndex selected, const QModelIndex)
{
    if(selected.isValid()) {
        QVariant bpid = _searchModel->record(selected.row()).value(SearchResults::Bpid);
        ui->trackInfos->updateInfos(_dbHelper->trackInformations(bpid));
    } else {
        ui->trackInfos->clearData();
    }
}

void MainWindow::updateLibraryActions()
{
    int numSel = _libraryModel->selectedIds().size();

    ui->actionLibraryDelete->setDisabled(numSel == 0);
    ui->actionSearch->setDisabled(numSel == 0);
    ui->actionRename->setDisabled(numSel == 0);
}

void MainWindow::updateSearchResultsActions()
{
    int numSel = ui->searchResultsView->selectionModel()->selectedRows().size();

    ui->actionSetDefaultResult->setDisabled(numSel == 0);
    ui->actionSearchResultDelete->setDisabled(numSel == 0);
}

void MainWindow::on_actionAbout_triggered()
{
    QDialog * container = new QDialog(this);
    Ui::AboutDialog about;
    about.setupUi(container);
    container->show();
}

void MainWindow::on_actionSearch_triggered()
{
    SearchWizard wizard(_libraryModel->selectedRecords().values());
    wizard.exec();
    _libraryModel->refresh();
}

void MainWindow::on_libraryView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu;
    contextMenu.addActions(QList<QAction*>() << ui->actionImport << ui->actionLibraryDelete);
    contextMenu.exec(ui->libraryView->mapToGlobal(pos));
}

void MainWindow::on_actionImport_triggered()
{
    //QString filters = "Audio tracks (*.wav *.flac *.mp3);;Playlists [not implemented] (*.nml *.m3u)";
    QString filters = "Audio tracks (*.wav *.flac *.mp3)";
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", "../tym/resources/examples/tracks", filters, 0, 0);
    if(! fileList.isEmpty()) {
        _dbHelper->importFiles(fileList);
    }
}

void MainWindow::on_actionLibraryDelete_triggered()
{
    QHash<int, QSqlRecord> selecteds = _libraryModel->selectedRecords();
    QList<int> rows;
    QStringList uids;

    QHashIterator<int, QSqlRecord> it(selecteds);
    while(it.hasNext()) {
        rows << it.next().key();
        uids << it.value().value(Library::Uid).toString();
    }
    _dbHelper->deleteFromLibrary(uids);
    _libraryModel->unselectRowsAndRefresh(rows);
}

void MainWindow::on_searchResultsView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu;
    contextMenu.addActions(QList<QAction*>() << ui->actionSetDefaultResult << ui->actionSearchResultDelete);
    contextMenu.exec(ui->searchResultsView->mapToGlobal(pos));
}

void MainWindow::on_actionSetDefaultResult_triggered()
{
    int row = ui->searchResultsView->selectionModel()->selectedRows().first().row();

    QString libId = _searchModel->data(_searchModel->index(row, SearchResults::LibId)).toString();
    QString bpid = _searchModel->data(_searchModel->index(row, SearchResults::Bpid)).toString();

    _dbHelper->setLibraryTrackReference(libId, bpid);
    ui->searchResultsView->selectRow(row);
}

void MainWindow::on_actionSearchResultDelete_triggered()
{
    int row = ui->searchResultsView->selectionModel()->selectedRows().first().row();

    QString libId = _searchModel->data(_searchModel->index(row, SearchResults::LibId)).toString();
    QString trackId = _searchModel->data(_searchModel->index(row, SearchResults::Bpid)).toString();
    _dbHelper->deleteSearchResult(libId, trackId);
    _libraryModel->refresh();
    ui->searchResultsView->selectRow(row-1);
}

void MainWindow::on_actionRename_triggered()
{
    RenameWizard wizard(_libraryModel->selectedRecords().values());
    if(wizard.exec() == SearchWizard::Rejected) {
        return;
    }
    _libraryModel->refresh();
}
