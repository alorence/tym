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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    defaultConsoleDisplaying(false),
    ui(new Ui::MainWindow),
    settings(new SettingsDialog(this)),
    searchProvider(settings, this),
    dbThread(new QThread(this))
{
    ui->setupUi(this);
    connect(ui->actionSettings, SIGNAL(triggered()), settings, SLOT(open()));

    if( ! BPDatabase::instance()->initialized()) {
        qCritical() << tr("Impossible to connect with database...");
        return;
    }

    dbThread->start();
    BPDatabase::instance()->moveToThread(dbThread);

    ui->progress->setVisible(false);
    connect(&searchProvider, SIGNAL(searchResultAvailable(QString,QVariant)), this, SLOT(updateProgressBar()));

    // Used to transfer fixed parameters to some slots
    generalMapper = new QSignalMapper(this);

    /**
     * Library
     */
    // Configure view
    ui->libraryView->setModel(BPDatabase::instance()->libraryModel());
    ui->libraryView->hideColumn(LibraryIndexes::Uid);
    ui->libraryView->hideColumn(LibraryIndexes::Bpid);
    ui->libraryView->resizeColumnsToContents();

    // Check rows in model when selection change on the view
    connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
            BPDatabase::instance()->libraryModel(), SLOT(updateCheckedRows(const QItemSelection&,const QItemSelection&)));
    // Select or deselect rows on the view when checkboxes are checked / unchecked
    connect(BPDatabase::instance()->libraryModel(), SIGNAL(rowCheckedOrUnchecked(QItemSelection,QItemSelectionModel::SelectionFlags)),
            ui->libraryView->selectionModel(), SLOT(select(QItemSelection,QItemSelectionModel::SelectionFlags)));
    // Set current selection index to last modified row
    connect(BPDatabase::instance()->libraryModel(), SIGNAL(rowCheckedOrUnchecked(QModelIndex,QItemSelectionModel::SelectionFlags)),
            ui->libraryView->selectionModel(), SLOT(setCurrentIndex(QModelIndex,QItemSelectionModel::SelectionFlags)));
    // Update search results view when selecting something in the library view
    connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            BPDatabase::instance(), SLOT(updateSearchResults(const QModelIndex&,const QModelIndex&)));

    /**
     * Search Results View
     */
    // Configure view
    ui->searchResultsView->setModel(BPDatabase::instance()->searchModel());
    ui->searchResultsView->hideColumn(SearchResultsIndexes::LibId);
    ui->searchResultsView->hideColumn(SearchResultsIndexes::Bpid);
    ui->searchResultsView->hideColumn(SearchResultsIndexes::DefaultFor);

    // When the selection change in library view, the search results view should be reconfigured.
    //connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
    //        generalMapper, SLOT(map()));
    // The first row (0) must be selected :
    //generalMapper->setMapping(ui->libraryView->selectionModel(), 0);
    //connect(generalMapper, SIGNAL(mapped(int)), ui->searchResultsView, SLOT(selectRow(int)));

    connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateSearchResultsSelection(QModelIndex,QModelIndex)));

    // Display informations about a track when selecting it in the view
    connect(ui->searchResultsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            this, SLOT(updateTrackInfos(QModelIndex,QModelIndex)));
    connect(ui->trackInfos, SIGNAL(downloadPicture(const QString&)),
            &searchProvider, SLOT(downloadTrackPicture(const QString&)));
    connect(&searchProvider, SIGNAL(pictureDownloadFinished(QString)),
            ui->trackInfos, SLOT(displayDownloadedPicture(QString)));

    /**
     * Actions
     */
    connect(&searchProvider, SIGNAL(searchResultAvailable(int,QVariant)), databaseUtil, SLOT(storeSearchResults(int,QVariant)));

}

MainWindow::~MainWindow()
{
    dbThread->exit();
    BPDatabase::instance()->deleteInstance();
    delete ui;
    delete generalMapper;
    delete console;
    dbThread->wait();
    dbThread->deleteLater();
}

void MainWindow::registerConsole(QWidget *c)
{
    console = c;
    ui->centralWidget->layout()->addWidget(c);
    connect(ui->actionToggleConsole, SIGNAL(toggled(bool)), console, SLOT(setVisible(bool)));

    ui->actionToggleConsole->setChecked(defaultConsoleDisplaying);
    console->setVisible(defaultConsoleDisplaying);
}

void MainWindow::updateSearchResultsSelection(QModelIndex, QModelIndex)
{
    if(BPDatabase::instance()->searchModel()->rowCount() > 0) {
        ui->searchResultsView->selectRow(0);
    } else {
        ui->searchResultsView->setCurrentIndex(QModelIndex());
    }
}

void MainWindow::updateTrackInfos(QModelIndex selected, QModelIndex deselected)
{
    if(selected.isValid()) {
        QVariant bpid = BPDatabase::instance()->searchModel()->record(selected.row()).value(SearchResultsIndexes::Bpid);
        ui->trackInfos->updateInfos(BPDatabase::instance()->trackInformations(bpid));
    } else {
        ui->trackInfos->clearData();
    }
}

void MainWindow::on_actionImport_triggered()
{
    //QString filters = "Audio tracks (*.wav *.flac *.mp3);;Playlists [not implemented] (*.nml *.m3u)";
    QString filters = "Audio tracks (*.wav *.flac *.mp3)";
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", "../tym/sources_files", filters, 0, 0);

    BPDatabase::instance()->importFiles(fileList);

    ui->libraryView->resizeColumnsToContents();
}

void MainWindow::on_actionSearch_triggered()
{
    SearchWizard wizard;
    if(wizard.exec() == SearchWizard::Rejected) {
        return;
    }

    PatternTool pt(wizard.pattern());
    QMap<QString, QMap<QString, QString> > parsedValueMap;

    QStringList interestingKeys;
    if(wizard.searchType() == SearchWizard::FromId) {
        interestingKeys << "bpid";
    } else {
        interestingKeys << "artists" << "title" << "remixers" << "name" << "mixname" << "label";
    }

    QPair<int, QSqlRecord> entry;
    foreach (entry, BPDatabase::instance()->libraryModel()->selectedRecords()) {
        QSqlRecord record = entry.second;

        QString filePath = record.value(LibraryIndexes::FilePath).toString();
        QString fileName = filePath.split(QDir::separator()).last();

        parsedValueMap[record.value(LibraryIndexes::Uid).toString()] = pt.parseValues(fileName, interestingKeys);
    }

    ui->progress->setVisible(true);
    ui->progress->setValue(ui->progress->minimum());

    QMap<QString, QString> * requestMap = new QMap<QString, QString>();
    if(wizard.searchType() == SearchWizard::FromId) {
        foreach(QString key, parsedValueMap.keys()) {
            requestMap->insert(key, parsedValueMap[key]["bpid"]);
        }
        ui->progress->setMaximum(requestMap->size());
        searchProvider.searchFromIds(requestMap);
    } else {
        foreach(QString key, parsedValueMap.keys()) {
            requestMap->insert(key, ((QStringList)parsedValueMap[key].values()).join(" "));
        }
        ui->progress->setMaximum(requestMap->size());
        searchProvider.searchFromName(requestMap);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    QDialog * container = new QDialog(this);
    Ui::AboutDialog about;
    about.setupUi(container);
    container->show();
}

void MainWindow::updateProgressBar()
{int newValue = ui->progress->value() + 1;
    ui->progress->setValue(newValue);

    if(newValue == ui->progress->maximum()) {
        ui->progress->setVisible(false);
    }
}

void MainWindow::on_actionDelete_triggered()
{
    QList<QPair<int, QSqlRecord> > selecteds = BPDatabase::instance()->libraryModel()->selectedRecords();
    QList<int> rows;
    QVariantList uids;
    QPair<int, QSqlRecord> elt;
    foreach(elt, selecteds) {
        rows << elt.first;
        uids << elt.second.value(LibraryIndexes::Uid);
    }
    BPDatabase::instance()->deleteFromLibrary(uids);
    BPDatabase::instance()->libraryModel()->unselectRowsAndRefresh(rows);
}
