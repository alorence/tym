#include "trackinfosview.h"
#include "ui_trackinfosview.h"

TrackInfosView::TrackInfosView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackInfosView)
{
    ui->setupUi(this);
}

TrackInfosView::~TrackInfosView()
{
    delete ui;
}

void TrackInfosView::updateInfos(QVariant & bpid)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM BPTracks WHERE bpid=:bpid");
    query.bindValue(":bpid", bpid);
    if( ! query.exec() ) {
        qWarning() << "Unable to get track informations :" << query.lastError().text();
    }

    query.next();
    query.nextResult();
    qDebug() << "Result size:" << query.size();
}

void TrackInfosView::clearData()
{
    qDebug() << "clear data";
}

