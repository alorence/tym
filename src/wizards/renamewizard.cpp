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

#include "renamewizard.h"
#include "ui_renamewizard.h"

RenameWizard::RenameWizard(QList<QPair<int, QSqlRecord> > selected, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::RenameWizard)
{
    ui->setupUi(this);

    on_patternSelection_currentIndexChanged(ui->patternSelection->currentIndex());

    ui->previewTable->setRowCount(selected.count());
    ui->previewTable->setColumnCount(3);

    QStringList bpids;

    QPair<int, QSqlRecord> elt;
    int row = 0;
    foreach(elt, selected) {
        QSqlRecord record = elt.second;

        QString filePath = record.value(LibraryIndexes::FilePath).toString();
        QString fileName = filePath.split(QDir::separator()).last();

        QString bpid = record.value(LibraryIndexes::Bpid).toString();;
        bpids << bpid;

        QTableWidgetItem *item = new QTableWidgetItem(bpid);
        ui->previewTable->setItem(row, 0, item);

        item = new QTableWidgetItem(fileName);
        ui->previewTable->setItem(row, 1, item);
        row++;
    }

    QSqlQuery tracksInfos = BPDatabase::instance()->tracksInformations(bpids);
    while(tracksInfos.next()) {
        tracksInformations[tracksInfos.value(TrackFullInfosIndexes::Bpid).toString()] = tracksInfos.record();
    }
    updateRenamePreview();
}

RenameWizard::~RenameWizard()
{
    delete ui;
}

void RenameWizard::updateRenamePreview()
{
    PatternTool patternTool(ui->pattern->text());

    for(int row = 0 ; row < ui->previewTable->rowCount() ; ++row) {
        QTableWidgetItem *item = new QTableWidgetItem(patternTool.stringFromPattern(tracksInformations[ui->previewTable->item(row, 0)->text()]));
        ui->previewTable->setItem(row, 2, item);
    }
}

void RenameWizard::on_patternSelection_currentIndexChanged(int index)
{
    switch(index) {
    case 0:
        ui->pattern->setText("%ARTISTS% - %NAME% (%MIXNAME%)");
        break;
    case 1:
        ui->pattern->setText("%ARTISTS% - %TITLE%");
        break;
    case 2:
        ui->pattern->setText("%ARTISTS% (%LABEL%) - %TITLE%");
        break;
    }
    updateRenamePreview();
}
