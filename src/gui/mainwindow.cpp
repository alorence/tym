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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <Logger.h>
#include <WidgetAppender.h>

#include "about.h"
#include "commons.h"
#include "settingsdialog.h"
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
    _libStatusUpdateThread(new QThread()),
    _aboutDialog(new About(this))
{
    ui->setupUi(this);

    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, _aboutDialog, &QDialog::show);

    // Configure console message displaying
    connect(ui->actionToggleConsole, &QAction::toggled, this, &MainWindow::toggleConsoleDisplaying);
    ui->actionToggleConsole->setChecked(_defaultConsoleDisplaying);
    toggleConsoleDisplaying(_defaultConsoleDisplaying);

    WidgetAppender* widgetAppender = new WidgetAppender(ui->outputConsole);
    widgetAppender->setFormat("%m\n");
    Logger::registerAppender(widgetAppender);

    if( ! _dbHelper->initialized()) {
        LOG_ERROR(tr("Impossible to connect with database..."));
        return;
    }

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
    _searchModel->setFilter("libId=NULL");
    _searchModel->select();
    // Configure view
    ui->searchResultsView->setModel(_searchModel);
    ui->searchResultsView->hideColumn(SearchResults::LibId);
    ui->searchResultsView->hideColumn(SearchResults::Bpid);
    ui->searchResultsView->hideColumn(SearchResults::DefaultFor);

    // Ensure both library and search results view are always focused together
    ui->libraryView->setFocusProxy(ui->searchResultsView);

    // Select or deselect rows on the view when checkboxes are checked / unchecked
    connect(_libraryModel, SIGNAL(requestSelectRows(QItemSelection,QItemSelectionModel::SelectionFlags)),
            ui->libraryView->selectionModel(), SLOT(select(QItemSelection,QItemSelectionModel::SelectionFlags)));

    // Set current selection index to last modified row
    connect(_libraryModel, SIGNAL(requestChangeCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)),
            ui->libraryView->selectionModel(), SLOT(setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)));

    // Check rows in model when selection change on the view
    connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            _libraryModel, SLOT(updateCheckedRows(QItemSelection,QItemSelection)));

    // Set actions menu/buttons as enabled/disabled folowing library selection
    connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(updateLibraryActions()));

    // Update search results view when selecting something in the library view
    connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateSearchResults(QModelIndex,QModelIndex)));

    // Display informations about a track when selecting it in the view
    connect(ui->searchResultsView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateTrackInfos(QModelIndex,QModelIndex)));

    // TODO: Maybe useless when LibraryModel::refresh(int row) will work
    connect(_dbHelper, SIGNAL(referenceForTrackUpdated(QString)),
            _searchModel, SLOT(refresh(QString)));

    // Set actions menu/buttons as enabled/disabled folowing library selection
    connect(ui->searchResultsView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(updateSearchResultsActions()));

    // Download pictures when needed
    connect(ui->trackInfos, SIGNAL(downloadPicture(QString)),
            _pictureDownloader, SLOT(downloadTrackPicture(QString)));
    connect(_pictureDownloader, SIGNAL(pictureDownloadFinished(QString)),
            ui->trackInfos, SLOT(displayDownloadedPicture(QString)));

    connect(_dbHelper, SIGNAL(libraryEntryUpdated(QString)),
            _libraryModel, SLOT(refresh()));

    // Configure actions for selecting groups in library
    _selectionActions[LibraryModel::AllTracks] = "All tracks";
    _selectionActions[LibraryModel::NewTracks] = "New";
    _selectionActions[LibraryModel::MissingTracks] = "Missing";
    _selectionActions[LibraryModel::LinkedTracks] = "Linked to a result";

    QMapIterator<LibraryModel::GroupSelection, QString> it(_selectionActions);

    ui->selectionCombo->addItem("", -1);
    while(it.hasNext()) {
        LibraryModel::GroupSelection id = it.next().key();
        QString label = it.value();

        ui->selectionCombo->addItem(label, id);

        QAction * action = new QAction(label, ui->libraryView);
        _selectionMapper.setMapping(action, id);
        connect(action, SIGNAL(triggered()), &_selectionMapper, SLOT(map()));

        _selectActionsList << action;
    }
    // Connect combobox to the slot
    connect(ui->selectionCombo, SIGNAL(activated(int)),
            this, SLOT(selectSpecificLibraryElements(int)));
    // Connect context menu, via the QSignalMapper
    connect(&_selectionMapper, SIGNAL(mapped(int)),
            _libraryModel, SLOT(selectSpecificGroup(int)));

    // Configure settings management
    connect(ui->actionSettings, &QAction::triggered, _settings, &QDialog::open);
    connect(_settings, &QDialog::accepted, this, &MainWindow::updateSettings);
    connect(_settings, &QDialog::accepted, _libraryModel, &LibraryModel::updateSettings);

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
    delete _libraryModel;
    delete _searchModel;
    delete _pictureDownloader;
    delete _settings;
    _libStatusUpdateThread->wait();
    _libStatusUpdateThread->deleteLater();
    foreach(QAction *action, _selectActionsList) {
        action->deleteLater();
    }
}

void MainWindow::updateSettings()
{
    QSettings settings;
    bool proxyEnabled = settings.value(TYM_PATH_PROXY_ENABLED, TYM_DEFAULT_PROXY_ENABLED).toBool();
    if(proxyEnabled) {

        QString proxyHost = settings.value(TYM_PATH_PROXY_HOST, TYM_DEFAULT_PROXY_HOST).toString();
        quint16 proxyPort = settings.value(TYM_PATH_PROXY_PORT, TYM_DEFAULT_PROXY_PORT).toUInt();
        QString proxyUser = settings.value(TYM_PATH_PROXY_USER, TYM_DEFAULT_PROXY_USER).toString();
        QString proxyPass = settings.value(TYM_PATH_PROXY_PWD, TYM_DEFAULT_PROXY_PWD).toString();

        QNetworkProxy proxy(QNetworkProxy::Socks5Proxy, proxyHost, proxyPort, proxyUser, proxyPass);
        QNetworkProxy::setApplicationProxy(proxy);
    } else if(QNetworkProxy::applicationProxy().type() != QNetworkProxy::NoProxy) {
        QNetworkProxy proxy(QNetworkProxy::NoProxy);
        QNetworkProxy::setApplicationProxy(proxy);
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{

    QStringList filteredFiles;
    foreach(QUrl url, event->mimeData()->urls()) {

        if(! url.isLocalFile()) continue;

        QFileInfo entry(url.toLocalFile());
        if( ! entry.exists()) continue;

        foreach(QFileInfo file, filteredFileList(entry)) {
            filteredFiles.append(file.absoluteFilePath());
        }
    }

    _dbHelper->importFiles(filteredFiles);
}

void MainWindow::showEvent(QShowEvent *)
{
    // Configure libraryView filePath column to take as space as possible
    QHeaderView *horizHeader = ui->libraryView->horizontalHeader();
    // Ensure 2 last colums have the default section size
    ui->libraryView->setColumnWidth(Library::FilePath, horizHeader->width() - 2 * horizHeader->defaultSectionSize());
}

void MainWindow::toggleConsoleDisplaying(bool show) const
{
    if(show) {
        ui->actionToggleConsole->setText(tr("Hide console"));
    } else {
        ui->actionToggleConsole->setText(tr("Show console"));
    }

    ui->outputConsole->setVisible(show);
}

void MainWindow::updateSearchResults(const QModelIndex & selected, const QModelIndex &)
{
    QSqlRecord current = _libraryModel->record(selected.row());
    QString libId = current.value(Library::Uid).toString();
    _searchModel->setFilter("libId=" + libId);

    if(_searchModel->rowCount()) {
        QString bpid = current.value(Library::Bpid).toString();
        if( ! bpid.isEmpty()) {
            for(int i = 0 ; i < _searchModel->rowCount() ; ++i) {
                if(_searchModel->record(i).value(SearchResults::Bpid).toString() == bpid) {
                    ui->searchResultsView->selectRow(i);
                    break;
                }
            }
        } else {
            ui->searchResultsView->selectRow(0);
        }

    } else {
        //When no more results are displayed, we must clear the track infos widget
        ui->trackInfos->clearData();
    }
}

void MainWindow::updateTrackInfos(const QModelIndex &selected, const QModelIndex&)
{
    QString bpid = _searchModel->record(selected.row()).value(SearchResults::Bpid).toString();
    ui->trackInfos->updateInfos(_dbHelper->trackInformations(bpid));
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

void MainWindow::selectSpecificLibraryElements(int index)
{
    int group = ui->selectionCombo->itemData(index).toInt();
    if(group != -1) {
        _libraryModel->selectSpecificGroup(group);
    }
    ui->libraryView->setFocus();
}

void MainWindow::on_actionSearch_triggered()
{
    SearchWizard wizard(_libraryModel->selectedRecords().values());
    if(wizard.exec() == QWizard::Rejected) {
        return;
    }
    _libraryModel->refresh();
}

void MainWindow::on_libraryView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu;
    QMenu *selectMenu = contextMenu.addMenu(tr("Select"));
    foreach(QAction *selectAction, _selectActionsList) {
        selectMenu->addAction(selectAction);
    }

    contextMenu.addSeparator();
    contextMenu.addActions(QList<QAction*>() << ui->actionImport << ui->actionLibraryDelete);
    contextMenu.exec(ui->libraryView->mapToGlobal(pos));
}

void MainWindow::on_actionImport_triggered()
{
    QSettings settings;
    QString searchDir = settings.value("lastOpenedDir", QStandardPaths::writableLocation(QStandardPaths::MusicLocation)).toString();

    //QString filters = "Audio tracks (*.wav *.flac *.mp3);;Playlists [not implemented] (*.nml *.m3u)";
    QString wildcards = TYM_SUPPORTED_SUFFIXES.join(" ");

    QString filters = "Audio tracks ("+wildcards+")";

    // TODO: Implements a specific class to open both files and dirctories
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", searchDir, filters, 0, 0);
    if(! fileList.isEmpty()) {
        _dbHelper->importFiles(fileList);

        QFileInfo f(fileList.first());
        settings.setValue("lastOpenedDir", f.absolutePath());
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
    _dbHelper->deleteLibraryEntry(uids);
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
    if(wizard.exec() == QWizard::Rejected) {
        return;
    }
    _libraryModel->refresh();
}

const QFileInfoList MainWindow::filteredFileList(const QFileInfo &entry) const
{
    QFileInfoList result;

    if(entry.isDir()) {
        foreach(QFileInfo toto, QDir(entry.absoluteFilePath()).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
            result.append(filteredFileList(toto));
        }

    } else if(entry.isFile()) {
        foreach(QString wildcard, TYM_SUPPORTED_SUFFIXES) {
            if(entry.fileName().contains(QRegExp(wildcard, Qt::CaseInsensitive, QRegExp::Wildcard))) {
                result.append(entry);
            }
        }
    }

    return result;
}
