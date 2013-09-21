/*****************************************************************************
Copyright 2012-2013 Antoine Lorence. All right reserved.

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

#include <QNetworkProxy>
#include <QDragEnterEvent>
#include <QFileDialog>

#include <Logger.h>
#include "about.h"
#include "commons.h"
#include "settingsdialog.h"
#include "dbaccess/searchresultsmodel.h"
#include "dbaccess/bpdatabase.h"
#include "tasks/rename/renameconfigurator.h"
#include "tasks/export/exportconfigurator.h"
#include "tasks/search/searchconfigurator.h"
#include "tasks/libupdater/librarystatusupdater.h"
#include "tools/patterntool.h"
#include "tools/langmanager.h"
#include "network/picturedownloader.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _defaultConsoleDisplaying(false),
    ui(new Ui::MainWindow),
    _aboutDialog(new About(this)),
    _settings(new SettingsDialog(this)),
    _dbHelper(new BPDatabase),
    _pictureDownloader(new PictureDownloader(this)),
    _libStatusUpdateThread(new QThread()),
    _networkStatus(new QLabel(this))
{
    ui->setupUi(this);

    // More space for library view than right panel
    ui->horizontalSplitter->setStretchFactor(0, 4);
    ui->horizontalSplitter->setStretchFactor(1, 1);

    // TODO: not very useful
    if( ! _dbHelper->initialized()) {
        LOG_ERROR(QString("Impossible to connect with database..."));
        return;
    }

    // Init some stuff. Order IS important
    initLibraryModelView();
    initSearchResultsModelView();
    initActions();
    initGroupSelectionHelpers();

    // Configure thread to update library entries status
    Task* libStatusUpdateTask = new LibraryStatusUpdater();
    libStatusUpdateTask->moveToThread(_libStatusUpdateThread);
    connect(_libStatusUpdateThread, &QThread::started, libStatusUpdateTask, &Task::run);
    connect(libStatusUpdateTask, &Task::finished, _libraryModel, &LibraryModel::refresh);
    connect(_libStatusUpdateThread, &QThread::destroyed, libStatusUpdateTask, &Task::deleteLater);

    // Configure status bar
    ui->statusBar->addWidget(_networkStatus, 2);
    connect(O1Beatport::instance(), &O1Beatport::statusChanged, this, &MainWindow::updateNetworkStatus);
    connect(O1Beatport::instance(), &O1Beatport::statusChanged, this, &MainWindow::updateLoginLogoutLabel);

    // Update library entries status (missing, etc.) at startup
    _libStatusUpdateThread->start();

    // Update actions status (disabled/enabled) at startup
    updateLibraryActions();
    updateSearchResultsActions();
    updateSettings();
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
    delete _networkStatus;
    qDeleteAll(_selectActionsList);
}

/********************************************************
 * Events reimplemented from parent classes [protected] *
 ********************************************************/
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
    QWidget::dragEnterEvent(event);
}

void MainWindow::dropEvent(QDropEvent *event)
{

    QStringList filteredFiles;
    for(QUrl url : event->mimeData()->urls()) {

        if(! url.isLocalFile()) continue;

        QFileInfo entry(url.toLocalFile());
        if( ! entry.exists()) continue;

        for(QFileInfo file : filteredFileList(entry)) {
            filteredFiles.append(file.absoluteFilePath());
        }
    }

    _dbHelper->importFiles(filteredFiles);
    QWidget::dropEvent(event);
}

void MainWindow::showEvent(QShowEvent *e)
{
    // Configure libraryView filePath column to take as space as possible
    QHeaderView *horizHeader = ui->libraryView->header();
    // Ensure 1st column take as most space as possible
    ui->libraryView->setColumnWidth(Library::Name,
                                    horizHeader->width() - 3 * horizHeader->defaultSectionSize());
    // Enlarge last column (same size as first one)
    ui->libraryView->setColumnWidth(Library::Infos, ui->libraryView->columnWidth(Library::Name));

    // Customize toolbar to display login/logout button on the right
    QWidget *spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->toolBar->insertWidget(ui->actionLoginLogout, spacer);

    // Initialize label for login/logout button
    updateNetworkStatus(O1Beatport::InitialState);
    updateLoginLogoutLabel();

    // Try to link for the first time
    O1Beatport::instance()->link();

    QSettings settings;
    if(settings.contains(TYM_WINDOW_GEOMETRY)) {
        restoreGeometry(settings.value(TYM_WINDOW_GEOMETRY).toByteArray());
    }

    QWidget::showEvent(e);
}

void MainWindow::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange) {
        // Save the current library root path (/Users/John/Music/mp3)
        QString rootTextSave = ui->rootDirLabel->text();
        // Retranslate all
        ui->retranslateUi(this);
        // Restore the path for the library root dir
        ui->rootDirLabel->setText(rootTextSave);
        // By default, login/logout button has a false text. Prevent from
        // redisplaying this one after a change on current language
        updateLoginLogoutLabel();
    }
    QWidget::changeEvent(e);
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    QSettings settings;
    settings.setValue(TYM_WINDOW_GEOMETRY, saveGeometry());

    QWidget::closeEvent(e);
}

/*********************************************************************
 * Private slots, used internally with signal called by user actions *
 *********************************************************************/
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

void MainWindow::updateNetworkStatus(O1Beatport::Status status)
{
    switch (status) {
    case O1Beatport::APIKeysMissing:
        _networkStatus->setText(tr("Tag Your Music is misconfigured. Connection to "
                                   "Beatport API is impossible"));
        ui->actionLoginLogout->setDisabled(true);
        break;
    case O1Beatport::Linked:
        _networkStatus->setText(tr("You are logged on Beatport and will be able to perform search"));
        break;
    case O1Beatport::InitialState:
    case O1Beatport::Notlinked:
    default:
        _networkStatus->setText(tr("You need to log in before searching on Beatport"));
        break;
    }
}

void MainWindow::updateLoginLogoutLabel()
{
    if(O1Beatport::instance()->linked()) {
        ui->actionLoginLogout->setText(tr("Log out"));
        ui->actionLoginLogout->setIcon(QIcon(":/action_icons/logout"));
    } else {
        ui->actionLoginLogout->setText(tr("Log in"));
        ui->actionLoginLogout->setIcon(QIcon(":/action_icons/login"));
    }
}

void MainWindow::updateSearchResults(const QModelIndex & selected, const QModelIndex &)
{
    QSqlRecord current = _libraryModel->record(selected);
    QString libId = current.value(Library::Uid).toString();
    _searchModel->setFilter("libId=" + libId);
    _searchModel->select();

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
        updateTrackInfos();
    }
}

void MainWindow::updateTrackInfos(const QModelIndex &selected, const QModelIndex&)
{
    // SearchResultsView can only have one item selected at a time
    if(selected.isValid()) {
        QString bpid = _searchModel->record(selected.row()).value(SearchResults::Bpid).toString();
        ui->trackInfos->updateInfos(_dbHelper->trackInformations(bpid));
    } else {
        ui->trackInfos->clearData();
    }
}

void MainWindow::updateLibraryActions()
{
    int numSel = ui->libraryView->selectionModel()->selectedRows().size();

    ui->actionRemove->setDisabled(numSel == 0);
    ui->actionSearch->setDisabled(numSel == 0);
    ui->actionRename->setDisabled(numSel == 0);
    ui->actionExport->setDisabled(numSel == 0);
}

void MainWindow::updateSearchResultsActions()
{
    int numSel = ui->searchResultsView->selectionModel()->selectedRows().size();

    ui->actionSetDefaultResult->setDisabled(numSel == 0);
    ui->actionSearchResultDelete->setDisabled(numSel == 0);
}

/**************************
 * Actions on LibraryView *
 **************************/
void MainWindow::beforeLibraryViewReset()
{
    // Save expanded folder, as a list of unique identifiers
    for(QModelIndex ind : _libraryModel->dirNodeModelIndexes()) {
        if(ui->libraryView->isExpanded(ind)) {
            _expandedItems << _libraryModel->data(ind, LibraryModel::UniquePathRole).toString();
        }
    }
}

void MainWindow::afterLibraryViewReset()
{
    // Update search results view when selecting something in the library view
    connect(ui->libraryView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &MainWindow::updateSearchResults);

    // Restore expanded items by searching them from their identifier
    for(QString uniquePathIdentifier : _expandedItems) {
        QModelIndexList matchList = _libraryModel->match(_libraryModel->index(0,0),
                                                         LibraryModel::UniquePathRole,
                                                         uniquePathIdentifier, 1,
                                                         Qt::MatchFixedString | Qt::MatchRecursive);
        if(matchList.size()) {
            QModelIndex item = matchList.at(0);
            do {
                ui->libraryView->expand(item);
                item = item.parent();
            } while (item.isValid());
        }
    }
    _expandedItems.clear();

    updateLibraryActions();
    updateSearchResults(ui->libraryView->selectionModel()->currentIndex());
}

void MainWindow::selectSpecificLibraryElements(int comboIndex)
{
    if(comboIndex == -1) return;

    LibraryModel::GroupSelection selectionGroup =
            (LibraryModel::GroupSelection) ui->groupSelectionCombo->itemData(comboIndex).toInt();
    if (selectionGroup == LibraryModel::Neither) {
        ui->libraryView->selectionModel()->clear();
    } else {
        ui->libraryView->selectionModel()->clear();
        QModelIndexList indexes = _libraryModel->indexesForGroup(selectionGroup);
        for(QModelIndex index : indexes) {
            ui->libraryView->selectionModel()->select(index,
                                            QItemSelectionModel::Select|QItemSelectionModel::Rows);

            // TODO: [settings] Add choice to use this or not
            if(_libraryModel->rowCount(index)) {
                QModelIndex toExpand = index;
                do {
                    ui->libraryView->expand(toExpand);
                    toExpand = _libraryModel->parent(toExpand);
                } while(toExpand.isValid());
            }
        }
    }
    ui->groupSelectionCombo->setCurrentIndex(-1);
}

void MainWindow::on_libraryView_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu;

    QMenu *selectMenu = contextMenu.addMenu(tr("Select tracks"));
    for(QAction *selectAction : _selectActionsList) {
        selectMenu->addAction(selectAction);
    }

    contextMenu.addSeparator();
    contextMenu.addActions(QList<QAction*>() << ui->actionImport << ui->actionRemove);
    contextMenu.exec(ui->libraryView->mapToGlobal(pos));
}

/************************************
 * Actions on SearchResultsView     *
 ************************************/
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

/************************************
 * Launch main tasks of the program *
 ************************************/
void MainWindow::on_actionSearch_triggered()
{
    QList<QSqlRecord> selectedRecords;
    _libraryModel->recordsForIndexes(ui->libraryView->selectionModel()->selectedRows(),
                                     selectedRecords);

    SearchConfigurator configurator(selectedRecords);
    if(configurator.exec() == QDialog::Rejected) {
        return;
    }

    TaskMonitor monitor(configurator.task());
    monitor.exec();

    _libraryModel->refresh();
}

void MainWindow::on_actionRename_triggered()
{
    QList<QSqlRecord> selectedRecords;
    _libraryModel->recordsForIndexes(ui->libraryView->selectionModel()->selectedRows(),
                                     selectedRecords);
    RenameConfigurator configurator(selectedRecords);
    if(configurator.exec() == QDialog::Rejected) {
        return;
    }

    TaskMonitor monitor(configurator.task());
    monitor.exec();

    _libraryModel->refresh();
}

void MainWindow::on_actionExport_triggered()

{
    QList<QSqlRecord> selectedRecords;
    _libraryModel->recordsForIndexes(ui->libraryView->selectionModel()->selectedRows(),
                                     selectedRecords);

    ExportConfigurator configurator(selectedRecords);
    if(configurator.exec() == QDialog::Rejected) {
        return;
    }

    TaskMonitor monitor(configurator.task());
    monitor.exec();
}

void MainWindow::on_actionImport_triggered()
{
    QSettings settings;
    QString searchDir = settings.value("lastOpenedDir", QStandardPaths::writableLocation(QStandardPaths::MusicLocation)).toString();

    //QString filters = "Audio tracks (*.wav *.flac *.mp3);;Playlists [not implemented] (*.nml *.m3u)";
    QString wildcards = TYM_SUPPORTED_SUFFIXES.join(" ");

    QString filters = "Audio tracks ("+wildcards+")";

    // TODO: Implements a specific class to open both files and dirctories
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", searchDir, filters, nullptr, nullptr);
    if(! fileList.isEmpty()) {
        _dbHelper->importFiles(fileList);

        QFileInfo f(fileList.first());
        settings.setValue("lastOpenedDir", f.absolutePath());
    }

    _libraryModel->refresh();
}

void MainWindow::on_actionRemove_triggered()
{
    QStringList selectedUids;
    _libraryModel->uidsForIndexes(ui->libraryView->selectionModel()->selectedRows(),
                                  selectedUids);
    _dbHelper->deleteLibraryEntry(selectedUids);
    _libraryModel->refresh();
}

void MainWindow::on_actionLoginLogout_triggered()
{
    if(O1Beatport::instance()->linked()) {
        O1Beatport::instance()->unlink();
    } else {
        O1Beatport::instance()->link();
    }
}

/*******************************
 * Private methods (utilities) *
 *******************************/
void MainWindow::initActions()
{
    // Connect some basic actions
    connect(ui->actionClose, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionAbout, &QAction::triggered, _aboutDialog, &QDialog::show);

    // Initialize default shortcuts for import and remove actions
    ui->actionRemove->setShortcut(QKeySequence::Delete);
    ui->actionImport->setShortcut(QKeySequence::Open);

    // Configure settings dialog
    connect(ui->actionSettings, &QAction::triggered, _settings, &QDialog::open);
    connect(_settings, &QDialog::accepted, this, &MainWindow::updateSettings);
    connect(_settings, &QDialog::accepted, _libraryModel, &LibraryModel::updateSettings);
    connect(_settings, &QDialog::accepted, LangManager::instance(), &LangManager::updateTranslationsFromSettings);

    // Other actions are connected automatically (on_actionXxx_trigered())
}

void MainWindow::initGroupSelectionHelpers()
{
    QList<QPair<LibraryModel::GroupSelection,QString>> selectionGrps;

    // Set the list of all supported actions
    selectionGrps << qMakePair(LibraryModel::AllTracks, tr("All"));
    selectionGrps << qMakePair(LibraryModel::Neither, tr("Neither"));
    selectionGrps << qMakePair(LibraryModel::NewTracks, tr("News"));
    selectionGrps << qMakePair(LibraryModel::MissingTracks, tr("Missing"));
    selectionGrps << qMakePair(LibraryModel::LinkedTracks, tr("With better result selected"));
    selectionGrps << qMakePair(LibraryModel::SearchedAndNotLinkedTracks, tr("No better result selected"));

    for(auto pair : selectionGrps) {
        int id = pair.first;
        QString label = pair.second;

        // Add each one to the comboBox
        ui->groupSelectionCombo->addItem(label, id);

        QAction * action = new QAction(label, ui->libraryView);
        _groupSelectionMapper.setMapping(action, id);
        connect(action, SIGNAL(triggered()), &_groupSelectionMapper, SLOT(map()));

        _selectActionsList << action;
    }
    // Connect combobox to the slot
    connect(ui->groupSelectionCombo, SIGNAL(activated(int)), this, SLOT(selectSpecificLibraryElements(int)));
    // Connect context menu, via the QSignalMapper
    connect(&_groupSelectionMapper, SIGNAL(mapped(int)), this, SLOT(selectSpecificLibraryElements(int)));
    // It is always a wrong entry displayed and selected by default
    ui->groupSelectionCombo->setCurrentIndex(-1);
}

void MainWindow::initLibraryModelView()
{
    // Configure Library Model and View
    _libraryModel = new LibraryModel(this);
    ui->libraryView->setModel(_libraryModel);

    // Update root dir text label
    connect(_libraryModel, &LibraryModel::rootPathChanged, ui->rootDirLabel, &QLabel::setText);

    // Reconfigure some view properties when it is refreshed
    connect(_libraryModel, &LibraryModel::modelAboutToBeReset, this, &MainWindow::beforeLibraryViewReset);
    connect(_libraryModel, &LibraryModel::modelReset, this, &MainWindow::afterLibraryViewReset);

    // Set actions menu/buttons as enabled/disabled folowing library selection
    connect(ui->libraryView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateLibraryActions);

}

void MainWindow::initSearchResultsModelView()
{
    // Configure search model
    _searchModel = new SearchResultsModel(this, _dbHelper->dbObject());
    _searchModel->setTable("SearchResultsHelper");
    _searchModel->setFilter("libId=NULL");
    _searchModel->select();

    // Configure search results view
    ui->searchResultsView->setModel(_searchModel);
    ui->searchResultsView->hideColumn(SearchResults::LibId);
    ui->searchResultsView->hideColumn(SearchResults::Bpid);
    ui->searchResultsView->hideColumn(SearchResults::DefaultFor);

    // Set actions menu/buttons as enabled/disabled folowing results list selection
    connect(ui->searchResultsView->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::updateSearchResultsActions);

    // Display information about a track when selecting it in the view
    connect(ui->searchResultsView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::updateTrackInfos);

    // Download pictures when needed
    connect(ui->trackInfos, &TrackInfosView::needDownloadPicture,
            _pictureDownloader, &PictureDownloader::downloadTrackPicture);
    connect(_pictureDownloader, &PictureDownloader::pictureDownloadFinished,
            ui->trackInfos, &TrackInfosView::displayDownloadedPicture);
}

const QFileInfoList MainWindow::filteredFileList(const QFileInfo &entry) const
{
    QFileInfoList result;

    if(entry.isDir()) {
        for(QFileInfo dirEntry : QDir(entry.absoluteFilePath()).entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot)) {
            result.append(filteredFileList(dirEntry));
        }

    } else if(entry.isFile()) {
        for(QString wildcard : TYM_SUPPORTED_SUFFIXES) {
            if(entry.fileName().contains(QRegExp(wildcard, Qt::CaseInsensitive, QRegExp::Wildcard))) {
                result.append(entry);
            }
        }
    }

    return result;
}
