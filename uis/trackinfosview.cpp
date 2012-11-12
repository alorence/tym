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

void TrackInfosView::updateInfos(QSqlRecord & result)
{
    ui->d_artists->setText(result.value(0).toString());
    ui->d_remixers->setText(result.value(1).toString());
    //ui->d_genres->setText(query.value(2).toString());
    ui->d_label->setText(result.value(3).toString());
    // No BPID lineEdit for now ui->d_->setText(query.value(4).toString());
    ui->d_name->setText(result.value(5).toString());
    ui->d_mixname->setText(result.value(6).toString());
    ui->d_title->setText(result.value(7).toString());
    //ui->d_labelNumber->setText(query.value(8).toString());
    ui->d_key->setText(result.value(9).toString());
    ui->d_bpm->setText(result.value(10).toString());
    QDateTime d = QDateTime::fromTime_t(result.value(11).toInt());
    ui->d_releaseDate->setText(d.toString(tr("yyyy-MM-dd")));
    d = QDateTime::fromTime_t(result.value(12).toInt());
    ui->d_publishdate->setText(d.toString(tr("yyyy-MM-dd")));
    ui->d_price->setText(result.value(13).toString());
    ui->d_length->setText(result.value(14).toString());
    ui->d_release->setText(result.value(15).toString());

    // If localFile has not been registered
    if(result.value(17).toString().isEmpty()) {
        currentImageUrl = result.value(16).toString();
        emit downloadPicture(currentImageUrl);
    }
}

void TrackInfosView::displayDownloadedPicture(QString url, QString localPath)
{
    // TODO : save local path of the file in database

    if(url == currentImageUrl) {
        ui->imageArea->setPixmap(QPixmap(localPath));
    }
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

    ui->imageArea->clear();
    currentImageUrl = "";
}

