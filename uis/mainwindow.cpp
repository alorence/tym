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

        // emitted from onSelectionChanged()
        connect(ui->libraryView, SIGNAL(rowSelectedChanged(QList<int>)),
                dbUtil.libraryModel(), SLOT(setRowsChecked(QList<int>)));
        // emitted from setData()
        connect(dbUtil.libraryModel(), SIGNAL(rowChecked(int,bool)),
                ui->libraryView, SLOT(setRowSelectState(int,bool)));

        ui->libraryView->setModel(dbUtil.libraryModel());
        connect(this, SIGNAL(importFilesToLibrary(QStringList)),
                &dbUtil, SLOT(importFiles(QStringList)));

        ui->singleElementView->setMapping(dbUtil.libraryModel());
        connect(ui->libraryView, SIGNAL(rowSelectedChanged(QList<int>)),
                ui->singleElementView, SLOT(setValuesForRow(QList<int>)));

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
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", "D:/workspaces/qt/sources_files", filters, 0, 0);
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

    QPair<int, QSqlRecord> entry;
    foreach (entry, dbUtil.libraryModel()->selectedRecords()){
        int id = entry.first;
        QSqlRecord record = entry.second;

        QString filePath = record.value(LibraryIndexes::FilePath).toString();
        QString fileName = filePath.split(QDir::separator()).last();

        parsedValueMap[id] = pt.parseValues(fileName);
    }


    if(wizard.searchType() == SearchWizard::FromId) {
        QMap<int, QString> * requestMap = new QMap<int, QString>();
        foreach(int id, parsedValueMap.keys()) {
            requestMap->insert(id, parsedValueMap[id]["bpid"]);
        }
        searchProvider.searchFromIds(requestMap);
    } else {
        foreach(int id, parsedValueMap.keys()) {
            qDebug() << id << " : " << parsedValueMap[id];
        }
    }
}


