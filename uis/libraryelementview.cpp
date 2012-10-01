#include "libraryelementview.h"
#include "ui_libraryelementview.h"

LibraryElementView::LibraryElementView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LibraryElementView),
    mapper(new QDataWidgetMapper(this))
{
    ui->setupUi(this);

    connect(mapper, SIGNAL(currentIndexChanged(int)), SLOT(updateValuesForRow(int)));

//    widgets[BPTracksIndexes::Artists] = ui->d_artists;
    widgets[BPTracksIndexes::Title] = ui->d_title;
//    widgets[BPTracksIndexes::Remixers] = ui->d_remixers;
    widgets[BPTracksIndexes::Label] = ui->d_label;
    widgets[BPTracksIndexes::ReleaseDate] = ui->d_releaseDate;
    widgets[BPTracksIndexes::Bpm] = ui->d_bpm;
    widgets[BPTracksIndexes::Key] = ui->d_key;
}

LibraryElementView::~LibraryElementView()
{
    delete ui;
}

void LibraryElementView::setMapping(LibraryModel *model)
{
    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    foreach(int col, widgets.keys()) {
        mapper->addMapping(widgets[col], col);
    }
}

void LibraryElementView::setValuesForRow(QList<int> rows)
{
    if(rows.count() == 1) {
        mapper->setCurrentIndex(rows.at(0));
    } else {
        clearData();
    }
}

void LibraryElementView::clearData(QList<int> indexes)
{
    if(indexes.size() == 0)
        indexes = widgets.keys();
    foreach(int index, indexes) {
        widgets[index]->clear();
    }
}

void LibraryElementView::setWidgetsEnabled(bool enabled, QList<int> indexes)
{
    if(indexes.size() == 0)
        indexes = widgets.keys();
    foreach(int index, indexes) {
        widgets[index]->setEnabled(enabled);
    }
}

void LibraryElementView::updateValuesForRow(int)
{

}

