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
    ui->previewTable->setColumnCount(4);
    ui->previewTable->hideColumn(0);
    ui->previewTable->hideColumn(1);

    QStringList bpids;

    QPair<int, QSqlRecord> elt;
    int row = 0;
    foreach(elt, selected) {
        QSqlRecord record = elt.second;

        QString bpid = record.value(LibraryIndexes::Bpid).toString();
        if( ! bpid.isEmpty()) {
            bpids << bpid;
        }

        QTableWidgetItem *item = new QTableWidgetItem(bpid);
        ui->previewTable->setItem(row, 0, item);

        QFileInfo original(record.value(LibraryIndexes::FilePath).toString());

        item = new QTableWidgetItem(original.dir().canonicalPath());
        ui->previewTable->setItem(row, 1, item);

        item = new QTableWidgetItem(original.fileName());
        ui->previewTable->setItem(row, 2, item);
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
        QString bpid = ui->previewTable->item(row, 0)->text();

        QString itemText = "";

        if( ! bpid.isEmpty()) {
            QFileInfo original(ui->previewTable->item(row, 2)->text());
            QString newBaseName = patternTool.stringFromPattern(tracksInformations[bpid]);

            if(newBaseName == original.baseName()) {
                itemText = "<File already have the right name>";
            } else {
                itemText = newBaseName + '.' + original.suffix();
            }
        } else {
            itemText = "<This file has no track attached>";
        }
        QTableWidgetItem *item = new QTableWidgetItem(itemText);
        ui->previewTable->setItem(row, 3, item);
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


void RenameWizard::initializePage(int id)
{
    if(id == 1) {
        QRegExp renameValidityCheck("^<.*>$");

        for(int row = 0 ; row < ui->previewTable->rowCount() ; ++row) {
            // Rename only if the target filename is not "<...>"
            if( ! renameValidityCheck.exactMatch(ui->previewTable->item(row, 0)->text())) {
                QString dir = ui->previewTable->item(row, 1)->text();

                QFileInfo from(dir + QDir::separator() + ui->previewTable->item(row, 2)->text());
                if( ! from.exists()) {
                    qWarning() << tr("Error, file %1 does not exists, it can't be renamed.")
                                  .arg(from.canonicalFilePath());
                    continue;
                }

                QString to = dir + QDir::separator() + ui->previewTable->item(row, 3)->text();

                if(QFile::exists(to)) {
                    qWarning() << tr("Error when renaming file %1, file %2 already exists.")
                                  .arg(from.canonicalFilePath())
                                  .arg(to);
                    continue;
                }

                // TODO : Redirect messages to the renamePage's console with Logger
                if(QFile::rename(from.canonicalFilePath(), to)) {
                    qDebug() << tr("File %1 renamed to %2")
                                .arg(from.canonicalFilePath())
                                .arg(QFileInfo(to).fileName());
                } else {
                    qWarning() << tr("Error when renaming file %1 into %2")
                                  .arg(from.canonicalFilePath())
                                  .arg(to);
                }

                // TODO : update database
            }
        }
    }
}
