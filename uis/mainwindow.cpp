/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag your Library) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag your Library) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag your Library).  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    defaultConsoleDisplaying(false),
    ui(new Ui::MainWindow),
    settings(new SettingsDialog(this)),
    searchProvider(settings, this),
    databaseUtil(this)
{
    ui->setupUi(this);
    connect(ui->actionSettings, SIGNAL(triggered()), settings, SLOT(open()));

    if( ! BPDatabase::initialized()) {
        qCritical() << tr("Impossible to connect with database...");
        return;
    }

    ui->progress->setVisible(false);
    connect(&searchProvider, SIGNAL(searchResultAvailable(int,QVariant)), this, SLOT(updateProgressBar()));


    // Used to transfer fixed parameters to some slots
    generalMapper = new QSignalMapper(this);

    /**
     * Library
     */
    // Configure view
    ui->libraryView->setModel(databaseUtil.libraryModel());
    ui->libraryView->hideColumn(LibraryIndexes::Uid);
    ui->libraryView->hideColumn(LibraryIndexes::Bpid);
    ui->libraryView->resizeColumnsToContents();

    // Check rows in model when selection change on the view
    connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
            databaseUtil.libraryModel(), SLOT(updateCheckedRows(const QItemSelection&,const QItemSelection&)));
    // Select or deselect rows on the view when checkboxes are checked / unchecked
    connect(databaseUtil.libraryModel(), SIGNAL(rowCheckedOrUnchecked(QItemSelection,QItemSelectionModel::SelectionFlags)),
            ui->libraryView->selectionModel(), SLOT(select(QItemSelection,QItemSelectionModel::SelectionFlags)));
    // Update search results view when selecting something in the library view
    connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            &databaseUtil, SLOT(updateSearchResults(const QModelIndex&,const QModelIndex&)));

    /**
     * Search Results View
     */
    // Configure view
    ui->searchResultsView->setModel(databaseUtil.searchModel());

    // When the selection change in library view, the search results view should be reconfigured.
    connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            generalMapper, SLOT(map()));
    // The first column (0) must be hidden, the first row (0) must be selected :
    generalMapper->setMapping(ui->libraryView->selectionModel(), 0);
    connect(generalMapper, SIGNAL(mapped(int)), ui->searchResultsView, SLOT(selectRow(int)));
    connect(generalMapper, SIGNAL(mapped(int)), ui->searchResultsView, SLOT(hideColumn(int)));

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
    connect(ui->actionDelete, SIGNAL(triggered()), databaseUtil.libraryModel(), SLOT(deleteSelected()));
    connect(&searchProvider, SIGNAL(searchResultAvailable(int,QVariant)), &databaseUtil, SLOT(storeSearchResults(int,QVariant)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete generalMapper;
    delete console;
}

void MainWindow::registerConsole(QWidget *c)
{
    console = c;
    ui->centralWidget->layout()->addWidget(c);
    connect(ui->actionToggleConsole, SIGNAL(toggled(bool)), console, SLOT(setVisible(bool)));

    ui->actionToggleConsole->setChecked(defaultConsoleDisplaying);
    console->setVisible(defaultConsoleDisplaying);
}

void MainWindow::updateTrackInfos(QModelIndex selected, QModelIndex)
{
    if(selected.isValid()) {
        QVariant bpid = databaseUtil.searchModel()->record(selected.row()).value(SearchResultsIndexes::Bpid);
        ui->trackInfos->updateInfos(databaseUtil.trackInformations(bpid));
    } else {
        ui->trackInfos->clearData();
    }
}

void MainWindow::on_actionImport_triggered()
{
    //QString filters = "Audio tracks (*.wav *.flac *.mp3);;Playlists [not implemented] (*.nml *.m3u)";
    QString filters = "Audio tracks (*.wav *.flac *.mp3)";
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", "../tym/sources_files", filters, 0, 0);

    databaseUtil.importFiles(fileList);

    ui->libraryView->resizeColumnsToContents();
}

void MainWindow::on_actionSearch_triggered()
{
    SearchWizard wizard;
    if(wizard.exec() == SearchWizard::Rejected) {
        return;
    }

    PatternTool pt(wizard.pattern());
    QMap<int, QMap<QString, QString> > parsedValueMap;

    QStringList interestingKeys;
    if(wizard.searchType() == SearchWizard::FromId) {
        interestingKeys << "bpid";
    } else {
        interestingKeys << "artists" << "title" << "remixers" << "name" << "mixname" << "label";
    }

    QPair<int, QSqlRecord> entry;
    foreach (entry, databaseUtil.libraryModel()->selectedRecords()) {
        int id = entry.first;
        QSqlRecord record = entry.second;

        QString filePath = record.value(LibraryIndexes::FilePath).toString();
        QString fileName = filePath.split(QDir::separator()).last();

        parsedValueMap[id] = pt.parseValues(fileName, interestingKeys);
    }

    ui->progress->setVisible(true);
    ui->progress->setValue(ui->progress->minimum());

    QMap<int, QString> * requestMap = new QMap<int, QString>();
    if(wizard.searchType() == SearchWizard::FromId) {
        foreach(int id, parsedValueMap.keys()) {
            requestMap->insert(id, parsedValueMap[id]["bpid"]);
        }
        ui->progress->setMaximum(requestMap->size());
        searchProvider.searchFromIds(requestMap);
    } else {
        foreach(int id, parsedValueMap.keys()) {
            requestMap->insert(id, ((QStringList)parsedValueMap[id].values()).join(" "));
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
