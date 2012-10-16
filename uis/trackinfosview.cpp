#include "trackinfosview.h"
#include "ui_trackinfosview.h"

TrackInfosView::TrackInfosView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackInfosView)
{
    ui->setupUi(this);

//    QString req = "SELECT "
//            "";
//    QSqlQueryModel * model = new QSqlQueryModel(req);

}

TrackInfosView::~TrackInfosView()
{
    delete ui;
}

void TrackInfosView::updateValues(QModelIndex & selection)
{
//    selection.row()
}

void TrackInfosView::clearData()
{

}

