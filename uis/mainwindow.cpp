#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(new SettingsDialog(this)),
    searchProvider(settings, this),
    dbUtil(this)
{
    ui->setupUi(this);
    connect(ui->actionSettings, SIGNAL(triggered()), settings, SLOT(open()));

    if(dbUtil.version() != "-1") {
        ui->libraryView->setModel(dbUtil.libraryModel());
        ui->libraryView->hideColumn(0);

        connect(this, SIGNAL(importFilesToLibrary(QStringList)),
                &dbUtil, SLOT(importFiles(QStringList)));

        connect(ui->libraryView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
                dbUtil.libraryModel(), SLOT(updateCheckedRows(const QItemSelection&,const QItemSelection&)));
        connect(dbUtil.libraryModel(), SIGNAL(rowChecked(QItemSelection,QItemSelectionModel::SelectionFlags)),
                ui->libraryView->selectionModel(), SLOT(select(QItemSelection,QItemSelectionModel::SelectionFlags)));

        connect(ui->libraryView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                &dbUtil, SLOT(updateSearchResults(const QModelIndex&,const QModelIndex&)));


        ui->searchResultsView->setModel(dbUtil.searchModel());
        connect(ui->searchResultsView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
                this, SLOT(updateTrackInfos(QModelIndex,QModelIndex)));
        ui->searchResultsView->hideColumn(0);

        connect(ui->actionDelete, SIGNAL(triggered()), dbUtil.libraryModel(), SLOT(deleteSelected()));
        connect(&searchProvider, SIGNAL(searchResultAvailable(QMap<int,QVariant>)), &dbUtil, SLOT(storeSearchResults(QMap<int,QVariant>)));
    } else {
        qCritical() << tr("Impossible to connect to database...");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionImport_triggered()
{
    QString filters = "Audio tracks (*.wav *.flac *.mp3);;Playlists [not implemented] (*.nml *.m3u)";
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", "../bpmanager/sources_files", filters, 0, 0);
    emit importFilesToLibrary(fileList);
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
    foreach (entry, dbUtil.libraryModel()->selectedRecords()){
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

void MainWindow::updateTrackInfos(QModelIndex selected, QModelIndex)
{
    if(selected.isValid()) {
        QSqlQueryModel *model = dbUtil.searchModel();
        QVariant bpid = model->record(selected.row()).value(0);
        ui->trackInfos->updateInfos(bpid);
    } else {
        ui->trackInfos->clearData();
    }
}


void MainWindow::on_actionAbout_triggered()
{
    QDialog * container = new QDialog(this);
    Ui::AboutDialog about;
    about.setupUi(container);
    container->show();
}
