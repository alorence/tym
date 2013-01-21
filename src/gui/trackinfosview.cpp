/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag Your Music) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag Your Music) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag Your Music).  If not, see <http://www.gnu.org/licenses/>.
*/

#include "trackinfosview.h"
#include "ui_trackinfosview.h"

#include "commons.h"
#include "dbaccess/bpdatabase.h"

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

void TrackInfosView::updateInfos(QSqlRecord result)
{
    // TODO : Add unexisting fields to display informations
    ui->d_artists->setText(result.value(TrackFullInfos::Artists).toString());
    ui->d_remixers->setText(result.value(TrackFullInfos::Remixers).toString());
    //ui->d_genres->setText(query.value(TrackFullInfosIndexes::Genres).toString());
    ui->d_label->setText(result.value(TrackFullInfos::LabelName).toString());
    // No BPID lineEdit for now ui->d_->setText(query.value(TrackFullInfosIndexes::Bpid).toString());
    ui->d_name->setText(result.value(TrackFullInfos::TrackName).toString());
    ui->d_mixname->setText(result.value(TrackFullInfos::MixName).toString());
    ui->d_title->setText(result.value(TrackFullInfos::Title).toString());
    //ui->d_labelNumber->setText(query.value(8).toString());
    ui->d_key->setText(result.value(TrackFullInfos::Key).toString());
    ui->d_bpm->setText(result.value(TrackFullInfos::Bpm).toString());
    QDateTime d = QDateTime::fromTime_t(result.value(TrackFullInfos::ReleaseDate).toInt());
    ui->d_releaseDate->setText(d.toString(tr("yyyy-MM-dd")));
    d = QDateTime::fromTime_t(result.value(TrackFullInfos::PublishDate).toInt());
    ui->d_publishdate->setText(d.toString(tr("yyyy-MM-dd")));
    ui->d_price->setText(result.value(TrackFullInfos::Price).toString());
    ui->d_length->setText(result.value(TrackFullInfos::Length).toString());
    ui->d_release->setText(result.value(TrackFullInfos::Release).toString());

    _currentPictureId = result.value(TrackFullInfos::PictureId).toString();

    QString picPath = Constants::picturesLocation() + QDir::separator() + _currentPictureId + ".jpg";
    if( ! QFile(picPath).exists()) {
        emit downloadPicture(_currentPictureId);
    } else {
        displayDownloadedPicture(_currentPictureId);
    }
}

void TrackInfosView::displayDownloadedPicture(QString picId)
{
    if(picId == _currentPictureId) {
        QString path(Constants::picturesLocation() + QDir::separator() + picId + ".jpg");
        ui->imageArea->setPixmap(QPixmap(path));
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
    _currentPictureId = "";
}

