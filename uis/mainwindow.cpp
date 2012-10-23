#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    defaultConsoleDisplaying(false),
    ui(new Ui::MainWindow),
    settings(new SettingsDialog(this)),
    searchProvider(settings, this),
    dbUtil(this)
{
    ui->setupUi(this);
    connect(ui->actionSettings, SIGNAL(triggered()), settings, SLOT(open()));

    if(dbUtil.version() != "-1") {
        // Used to transfer fixed parameters to some slots
        generalMapper = new QSignalMapper(this);

        /**
         * Library
         */
        // Configure view
        ui->libraryView->setModel(dbUtil.libraryModel());
        ui->libraryView->hideColumn(LibraryIndexes::Uid);
        ui->libraryView->hideColumn(LibraryIndexes::Bpid);
        ui->libraryView->resizeColumnsToContents();

        // Check rows in model when selection change on the view
        connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                dbUtil.libraryModel(), SLOT(updateCheckedRows(const QItemSelection&,const QItemSelection&)));
        // Select or deselect rows on the view when checkboxes are checked / unchecked
        connect(dbUtil.libraryModel(), SIGNAL(rowCheckedOrUnchecked(QItemSelection,QItemSelectionModel::SelectionFlags)),
                ui->libraryView->selectionModel(), SLOT(select(QItemSelection,QItemSelectionModel::SelectionFlags)));
        // Update search results view when selecting something in the library view
        connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                &dbUtil, SLOT(updateSearchResults(const QModelIndex&,const QModelIndex&)));

        /**
         * Search Results
         */
        // Configure view
        ui->searchResultsView->setModel(dbUtil.searchModel());

        // Display informations about a track when selecting it in the view
        connect(ui->searchResultsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(updateTrackInfos(QModelIndex,QModelIndex)));

        // When the selection change in library view, the search results view should be reconfigured.
        connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                generalMapper, SLOT(map()));
        // The first column (0) must be hidden, the first row must be selected :
        generalMapper->setMapping(ui->libraryView->selectionModel(), 0);
        connect(generalMapper, SIGNAL(mapped(int)), ui->searchResultsView, SLOT(selectRow(int)));
        connect(generalMapper, SIGNAL(mapped(int)), ui->searchResultsView, SLOT(hideColumn(int)));

        /**
         * Actions
         */
        connect(ui->actionDelete, SIGNAL(triggered()), dbUtil.libraryModel(), SLOT(deleteSelected()));
        connect(&searchProvider, SIGNAL(searchResultAvailable(QMap<int,QVariant>)), &dbUtil, SLOT(storeSearchResults(QMap<int,QVariant>)));
    } else {
        qCritical() << tr("Impossible to connect to database...");
    }
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
        QVariant bpid = dbUtil.searchModel()->record(selected.row()).value(SearchResultsIndexes::Bpid);
        ui->trackInfos->updateInfos(bpid);
    } else {
        ui->trackInfos->clearData();
    }
}

void MainWindow::on_actionImport_triggered()
{
    QString filters = "Audio tracks (*.wav *.flac *.mp3);;Playlists [not implemented] (*.nml *.m3u)";
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", "../bpmanager/sources_files", filters, 0, 0);

    dbUtil.importFiles(fileList);
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
    foreach (entry, dbUtil.libraryModel()->selectedRecords()) {
        int id = entry.first;
        QSqlRecord record = entry.second;

        QString filePath = record.value(LibraryIndexes::FilePath).toString();
        QString fileName = filePath.split(QDir::separator()).last();

        parsedValueMap[id] = pt.parseValues(fileName, interestingKeys);
    }

    QMap<int, QString> * requestMap = new QMap<int, QString>();
    if(wizard.searchType() == SearchWizard::FromId) {
        foreach(int id, parsedValueMap.keys()) {
            requestMap->insert(id, parsedValueMap[id]["bpid"]);
        }
        searchProvider.searchFromIds(requestMap);
    } else {
        foreach(int id, parsedValueMap.keys()) {
            requestMap->insert(id, ((QStringList)parsedValueMap[id].values()).join(" "));
        }
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
