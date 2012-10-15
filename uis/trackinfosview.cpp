#include "trackinfosview.h"
#include "ui_trackinfosview.h"

TrackInfosView::TrackInfosView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackInfosView),
    mapper(new QDataWidgetMapper(this))
{
    ui->setupUi(this);

    connect(mapper, SIGNAL(currentIndexChanged(int)), SLOT(updateValuesForRow(int)));

//    widgets[BPTracksIndexes::Artists] = ui->d_artists;
    lineWidgets[BPTracksIndexes::Title] = ui->d_title;
//    widgets[BPTracksIndexes::Remixers] = ui->d_remixers;
    lineWidgets[BPTracksIndexes::Label] = ui->d_label;
    lineWidgets[BPTracksIndexes::ReleaseDate] = ui->d_releaseDate;
    lineWidgets[BPTracksIndexes::Bpm] = ui->d_bpm;
    lineWidgets[BPTracksIndexes::Key] = ui->d_key;
}

TrackInfosView::~TrackInfosView()
{
    delete ui;
}

void TrackInfosView::setMapping(LibraryModel *model)
{
    mapper->setModel(model);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);
    foreach(int col, lineWidgets.keys()) {
        mapper->addMapping(lineWidgets[col], col);
    }
}

void TrackInfosView::setValuesForRow(QList<int> rows)
{
    if(rows.count() == 1) {
        mapper->setCurrentIndex(rows.at(0));
    } else {
        clearData();
    }
}

void TrackInfosView::clearData(QList<int> indexes)
{
    if(indexes.size() == 0)
        indexes = lineWidgets.keys();
    foreach(int index, indexes) {
        lineWidgets[index]->clear();
    }
}

void TrackInfosView::setWidgetsEnabled(bool enabled, QList<int> indexes)
{
    if(indexes.size() == 0)
        indexes = lineWidgets.keys();
    foreach(int index, indexes) {
        lineWidgets[index]->setEnabled(enabled);
    }
}

void TrackInfosView::updateValuesForRow(int)
{

}

