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

#include "about.h"
#include "commons.h"
#include "settingsdialog.h"
#include "dbaccess/librarymodel.h"
#include "dbaccess/searchresultsmodel.h"
#include "dbaccess/bpdatabase.h"
#include "wizards/searchwizard.h"
#include "wizards/renamewizard.h"
#include "tools/patterntool.h"
#include "tools/picturedownloader.h"

#include "concretetasks/librarystatusupdater.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _defaultConsoleDisplaying(false),
    ui(new Ui::MainWindow),
    _settings(new SettingsDialog(this)),
    _pictureDownloader(new PictureDownloader(this)),
    _dbHelper(new BPDatabase),
    _libStatusUpdateThread(new QThread())
{
    ui->setupUi(this);
    connect(ui->actionSettings, SIGNAL(triggered()), _settings, SLOT(open()));

    // Configure console message displaying
    connect(ui->actionToggleConsole, SIGNAL(toggled(bool)), ui->outputConsole, SLOT(setVisible(bool)));
    ui->actionToggleConsole->setChecked(_defaultConsoleDisplaying);
    ui->outputConsole->setVisible(_defaultConsoleDisplaying);

    WidgetAppender* widgetAppender = new WidgetAppender(ui->outputConsole);
    widgetAppender->setFormat("%m\n");
    Logger::registerAppender(widgetAppender);

    if( ! _dbHelper->initialized()) {
        LOG_ERROR(tr("Impossible to connect with database..."));
        return;
    }

    // Used to transfer fixed parameters to some slots
    _generalMapper = new QSignalMapper(this);

    // Configure Library Model
    _libraryModel = new LibraryModel(this, _dbHelper->dbObject());
    _libraryModel->setTable("LibraryHelper");
    _libraryModel->select();
    // Configure view
    ui->libraryView->setModel(_libraryModel);
    ui->libraryView->hideColumn(Library::Uid);
    ui->libraryView->hideColumn(Library::Bpid);

    // Configure Search Model
    _searchModel = new SearchResultsModel(this, _dbHelper->dbObject());
    _searchModel->setTable("SearchResultsHelper");
    _searchModel->select();
    // Configure view
    ui->searchResultsView->setModel(_searchModel);
    ui->searchResultsView->hideColumn(SearchResults::LibId);
    ui->searchResultsView->hideColumn(SearchResults::Bpid);
    ui->searchResultsView->hideColumn(SearchResults::DefaultFor);

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
    // Set actions menu/buttons as enabled/disabled folowing library selection
    connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
            this, SLOT(updateLibraryActions()));

    // When the selection change in library view, the search results view should be reconfigured.
    connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            _generalMapper, SLOT(map()));
    // The first row (0) must be selected :
    _generalMapper->setMapping(ui->libraryView->selectionModel(), 0);
    connect(_generalMapper, SIGNAL(mapped(int)), ui->searchResultsView, SLOT(selectRow(int)));

    // Display informations about a track when selecting it in the view
    connect(ui->searchResultsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateTrackInfos(QModelIndex,QModelIndex)));
    connect(_dbHelper, SIGNAL(referenceForTrackUpdated(QString)),
            _searchModel, SLOT(refresh(QString)));
    // Set actions menu/buttons as enabled/disabled folowing library selection
    connect(ui->searchResultsView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
            this, SLOT(updateSearchResultsActions()));

    // Download pictures when needed
    connect(ui->trackInfos, SIGNAL(downloadPicture(const QString&)),
            _pictureDownloader, SLOT(downloadTrackPicture(const QString&)));
    connect(_pictureDownloader, SIGNAL(pictureDownloadFinished(QString)),
            ui->trackInfos, SLOT(displayDownloadedPicture(QString)));


    connect(_dbHelper, SIGNAL(libraryEntryUpdated(QString)),
            _libraryModel, SLOT(refresh()));

    // Configure thread to update library entries status
    Task* libStatusUpdateTask = new LibraryStatusUpdater();
    libStatusUpdateTask->moveToThread(_libStatusUpdateThread);
    connect(_libStatusUpdateThread, SIGNAL(started()), libStatusUpdateTask, SLOT(run()));
    connect(libStatusUpdateTask, SIGNAL(finished()), _libraryModel, SLOT(refresh()));
    connect(_libStatusUpdateThread, SIGNAL(destroyed()), libStatusUpdateTask, SLOT(deleteLater()));

    // Update library entries status (missing, etc.) at startup
    _libStatusUpdateThread->start();

    // Update actions status (disabled/enabled) at startup
    updateLibraryActions();
    updateSearchResultsActions();
}

MainWindow::~MainWindow()
{
    _libStatusUpdateThread->quit();
    _dbHelper->deleteLater();
    delete ui;
    delete _generalMapper;
    delete _libraryModel;
    delete _searchModel;
    delete _pictureDownloader;
    delete _settings;
    _libStatusUpdateThread->wait();
    _libStatusUpdateThread->deleteLater();
}

void MainWindow::show()
{
    QMainWindow::show();

    // Configure libraryView filePath column to take as space as possible
    QHeaderView *horizHeader = ui->libraryView->horizontalHeader();
    ui->libraryView->setColumnWidth(Library::FilePath, horizHeader->width() - 2 * horizHeader->defaultSectionSize());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{

    QFileInfoList files;
    foreach(QUrl url, event->mimeData()->urls()) {

        if(! url.isLocalFile()) continue;

        QFileInfo entry(url.toLocalFile());
        if( ! entry.exists()) continue;

        files.append(filteredFileList(entry));
    }

    QStringList filteredFiles;
    foreach(QFileInfo file, files) {
        filteredFiles.append(file.absoluteFilePath());
    }

    _dbHelper->importFiles(filteredFiles);
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
        QString bpid = _searchModel->record(selected.row()).value(SearchResults::Bpid).toString();
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
    About about;
    about.show();
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
    QSettings settings;
    QString searchDir = settings.value("general/lastOpenedDir", QStandardPaths::writableLocation(QStandardPaths::MusicLocation)).toString();

    //QString filters = "Audio tracks (*.wav *.flac *.mp3);;Playlists [not implemented] (*.nml *.m3u)";
    QString wildcards = Constants::fileSuffixesAccepted().join(" ");

    QString filters = "Audio tracks ("+wildcards+")";

    // TODO: implements a specific class to open both files and dirctories
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", searchDir, filters, 0, 0);
    if(! fileList.isEmpty()) {
        _dbHelper->importFiles(fileList);

        QFileInfo f(fileList.first());
        settings.setValue("general/lastOpenedDir", f.absolutePath());
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

QFileInfoList MainWindow::filteredFileList(QFileInfo entry)
{
    QFileInfoList result;

    if(entry.isDir()) {
        foreach(QFileInfo toto, QDir(entry.absoluteFilePath()).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
            result.append(filteredFileList(toto));
        }

    } else if(entry.isFile()) {
        foreach(QString wildcard, Constants::fileSuffixesAccepted()) {
            if(entry.fileName().contains(QRegExp(wildcard, Qt::CaseInsensitive, QRegExp::Wildcard))) {
                result.append(entry);
            }
        }
    }

    return result;
}
