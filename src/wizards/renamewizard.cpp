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

#include "Logger.h"
#include "WidgetAppender.h"

#include "commons.h"
#include "tools/patterntool.h"
#include "dbaccess/bpdatabase.h"
#include "concretetasks/renametask.h"

RenameWizard::RenameWizard(QList<QSqlRecord> selected, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::RenameWizard)
{
    ui->setupUi(this);

    _widgetAppender = new WidgetAppender(ui->outputConsole);
    _widgetAppender->setFormat("%m\n");
    Logger::registerAppender(_widgetAppender);

    on_patternSelection_currentIndexChanged(ui->patternSelection->currentIndex());

    ui->previewTable->setRowCount(selected.count());
    ui->previewTable->setColumnCount(sizeof(PreviewColumns));
    ui->previewTable->hideColumn(Bpid);
    ui->previewTable->hideColumn(Directory);

    QStringList bpids;

    int row = 0;
    foreach(QSqlRecord record, selected) {

        QString bpid = record.value(Library::Bpid).toString();
        if( ! bpid.isEmpty()) {
            bpids << bpid;
        }

        QTableWidgetItem *item = new QTableWidgetItem(bpid);
        ui->previewTable->setItem(row, Bpid, item);

        QFileInfo original(QDir::toNativeSeparators(record.value(Library::FilePath).toString()));

        item = new QTableWidgetItem(QDir::toNativeSeparators(original.dir().canonicalPath()));
        ui->previewTable->setItem(row, Directory, item);

        item = new QTableWidgetItem(original.fileName());
        ui->previewTable->setItem(row, OrigFileName, item);
        row++;
    }

    BPDatabase dbHelper("renameWizard");
    // BUG : Default result set manually are not immediatly reachable with this call
    {
        QSqlQuery tracksInfos = dbHelper.tracksInformations(bpids);
        while(tracksInfos.next()) {
            _tracksInformations[tracksInfos.value(TrackFullInfos::Bpid).toString()] = tracksInfos.record();
        }
    }
    updateRenamePreview();
}

RenameWizard::~RenameWizard()
{
    delete ui;
    Logger::unRegisterAppender(_widgetAppender);
    delete _widgetAppender;
}

void RenameWizard::updateRenamePreview()
{
    PatternTool patternTool(ui->pattern->text());

    for(int row = 0 ; row < ui->previewTable->rowCount() ; ++row) {
        QString bpid = ui->previewTable->item(row, Bpid)->text();

        QString itemText = "";

        if( ! bpid.isEmpty()) {
            QFileInfo original(ui->previewTable->item(row, OrigFileName)->text());
            QString newBaseName = patternTool.stringFromPattern(_tracksInformations[bpid]);

            if(newBaseName == original.baseName()) {
                itemText = "<File already have the right name>";
            } else {
                itemText = newBaseName + '.' + original.suffix();
            }
        } else {
            itemText = "<This file has no track attached>";
        }
        QTableWidgetItem *item = new QTableWidgetItem(itemText);
        ui->previewTable->setItem(row, TargetFileName, item);
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
    if(id == ResultPage) {

        QHash<QString, QString> renameMap;

        for(int row = 0 ; row < ui->previewTable->rowCount() ; ++row) {

            QString dir = ui->previewTable->item(row, Directory)->text();
            QString from(dir + QDir::separator() + ui->previewTable->item(row, OrigFileName)->text());
            QString to = dir + QDir::separator() + ui->previewTable->item(row, TargetFileName)->text();

            renameMap.insert(from, to);
        }

        RenameTask *task = new RenameTask(renameMap);
        QThreadPool::globalInstance()->tryStart(task);
    }
}
