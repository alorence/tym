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
    QSqlQuery query(BPDatabase::dbObject());
    QString queryString = "SELECT "
            "(SELECT DISTINCT group_concat( art.name, ', ') "
            "FROM BPArtists as art JOIN BPTracksArtistsLink as artl "
            "ON artl.artistId=art.bpid "
            "WHERE artl.trackId=tr.bpid) as artists, "
            "(SELECT DISTINCT group_concat( rmx.name, ', ') "
            "FROM BPArtists as rmx JOIN BPTracksRemixersLink as rmxl "
            "ON rmxl.artistId=rmx.bpid "
            "WHERE rmxl.trackId=tr.bpid) as remixers, "
            "(SELECT DISTINCT group_concat( genre.name, ', ') "
            "FROM BPGenres as genre JOIN BPTracksGenresLink as genrel "
            "ON genrel.genreId=genre.bpid "
            "WHERE genrel.trackId=tr.bpid) as genres, "
            "l.name as labelName, tr.* "
            "FROM BPTracks as tr JOIN BPLabels as l ON l.bpid = tr.label "
            "WHERE tr.bpid=:bpid";

    query.prepare(queryString);
    query.bindValue(":bpid", bpid);
    if( ! query.exec() ) {
        qWarning() << "Unable to get track informations :" << query.lastError().text();
    }

    query.next();
    //qDebug() << query.record();
    ui->d_artists->setText(query.record().value(0).toString());
    ui->d_remixers->setText(query.record().value(1).toString());
    //ui->d_genres->setText(query.record().value(2).toString());
    ui->d_label->setText(query.record().value(3).toString());
    // No BPID lineEdit for now ui->d_->setText(query.record().value(4).toString());
    ui->d_name->setText(query.record().value(5).toString());
    ui->d_mixname->setText(query.record().value(6).toString());
    ui->d_title->setText(query.record().value(7).toString());
    //ui->d_labelNumber->setText(query.record().value(8).toString());
    ui->d_key->setText(query.record().value(9).toString());
    ui->d_bpm->setText(query.record().value(10).toString());
    QDateTime d = QDateTime::fromTime_t(query.record().value(11).toInt());
    ui->d_releaseDate->setText(d.toString(tr("yyyy-MM-dd")));
    d = QDateTime::fromTime_t(query.record().value(12).toInt());
    ui->d_publishdate->setText(d.toString(tr("yyyy-MM-dd")));
    ui->d_price->setText(query.record().value(13).toString());
    ui->d_length->setText(query.record().value(14).toString());
    ui->d_release->setText(query.record().value(15).toString());

    // TODO : dl image and print here.
    // URL : query.record().value(15).toString()
    // ui->imageArea->setPixmap(load );
}

void TrackInfosView::clearData()
{
    ui->d_artists->clear();
    ui->d_remixers->clear();
    //ui->d_genres->clear();
    ui->d_label->clear();
    // No BPID lineEdit for now ui->d_->->clear();
    ui->d_name->clear();
    ui->d_mixname->clear();
    ui->d_title->clear();
    //ui->d_labelNumber->clear();
    ui->d_key->clear();
    ui->d_bpm->clear();
    ui->d_releaseDate->clear();
    ui->d_publishdate->clear();
    ui->d_price->clear();
    ui->d_length->clear();
    ui->d_release->clear();
}

