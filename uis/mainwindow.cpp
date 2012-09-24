#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    settings(new SettingsDialog(this)),
    searchProvider(settings, this)
{
    ui->setupUi(this);
    connect(ui->actionSettings, SIGNAL(triggered()), settings, SLOT(open()));

    BPDatabase db(this);
    if(db.version() != "-1") {
        libraryModel = new LibraryModel(this, db.dbObject());
        libraryModel->setTable("Library");
        libraryModel->setJoinMode(QSqlRelationalTableModel::LeftJoin);
        libraryModel->setRelation(LibraryIndexes::Bpid, QSqlRelation("BPTracks", "bpid", "title"));
        libraryModel->select();
        connect(ui->libraryView, SIGNAL(rowSelectedChanged(QList<int>)), libraryModel, SLOT(setRowsChecked(QList<int>)));
        connect(libraryModel, SIGNAL(rowChecked(int,bool)), ui->libraryView, SLOT(setRowSelectState(int,bool)));

        ui->libraryView->setModel(libraryModel);
        connect(this, SIGNAL(importFilesToLibrary(QStringList)),
                libraryModel, SLOT(importFiles(QStringList)));

        ui->singleElementView->setMapping(libraryModel);
        connect(ui->libraryView, SIGNAL(rowSelectedChanged(QList<int>)),
                ui->singleElementView, SLOT(setValuesForRow(QList<int>)));

        tracksModel = new QSqlRelationalTableModel(this, db.dbObject());
        tracksModel->setRelation(BPTracksIndexes::Label, QSqlRelation("BPLabels", "bpid", "name"));
        tracksModel->select();
    } else {
        qDebug() << "Impossible to connect to database...";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionImport_triggered()
{
    QString filters = "Audio tracks (*.wav *.flac *.mp3);;Playlists [not implemented] (*.nml *.m3u)";
    QStringList fileList = QFileDialog::getOpenFileNames(this, "Select files", "E:/Library", filters, 0, 0);
    emit importFilesToLibrary(fileList);
}

void MainWindow::on_actionSettings_triggered()
{
}

void MainWindow::on_actionSearch_triggered()
{

    searchProvider.searchFromIds(QStringList() << "15" << "20" << "12547");
}
