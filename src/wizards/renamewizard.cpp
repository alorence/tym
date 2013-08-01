/*****************************************************************************
Copyright 2012-2013 Antoine Lorence. All right reserved.

This file is part of TYM (Tag Your Music).

TYM (Tag Your Music) is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TYM (Tag Your Music) is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TYM (Tag Your Music). If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include "renamewizard.h"
#include "ui_renamewizard.h"

#include "Logger.h"
#include "WidgetAppender.h"

#include "commons.h"
#include "tools/utils.h"
#include "dbaccess/bpdatabase.h"
#include "tasks/rename/renametask.h"

RenameWizard::RenameWizard(QList<QSqlRecord> selected, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::RenameWizard),
    _filenameFormatter()
{
    ui->setupUi(this);

    // Configure console
    _widgetAppender = new WidgetAppender(ui->outputConsole);
    _widgetAppender->setFormat("%{message}\n");
    _widgetAppender->setDetailsLevel(Logger::Info);
    logger->registerAppender(_widgetAppender);

    // Configure pattern button
    _patternHelperButton = new PatternButton(_filenameFormatter, ui->pattern, this);
    ui->patternHorizLayout->addWidget(_patternHelperButton);
    _patternHelperButton->hide();

    // Initilize preview table
    on_patternSelection_currentIndexChanged(ui->patternSelection->currentIndex());

    ui->previewTable->setRowCount(selected.count());
    ui->previewTable->setColumnCount(2);

    QStringList headers;
    headers << tr("Original filename") << tr("New filename");
    ui->previewTable->setHorizontalHeaderLabels(headers);

    QStringList bpids;

    int row = 0;
    for(QSqlRecord record : selected) {

        QFileInfo original(record.value(Library::FilePath).toString());
        _fileInfosList[row] = original;

        QString bpid = record.value(Library::Bpid).toString();
        if( ! bpid.isEmpty()) {
            bpids << bpid;
        }

        QTableWidgetItem * item = new QTableWidgetItem(original.fileName());
        item->setData(Qt::UserRole, bpid);
        // Ensure original filename can't be edited
        item->setFlags( item->flags() & ~Qt::ItemIsEditable);
        ui->previewTable->setItem(row, OrigFileName, item);

        row++;
    }

    {
        BPDatabase dbHelper("renameWizard");
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
    logger->unRegisterAppender(_widgetAppender);
    delete _widgetAppender;
}

void RenameWizard::updateRenamePreview()
{
    _filenameFormatter.setPattern(ui->pattern->text());

    for(int row = 0 ; row < _fileInfosList.size() ; ++row) {

        QString bpid = ui->previewTable->item(row, OrigFileName)->data(Qt::UserRole).toString();

        QString itemText = "";
        bool shouldBeUsed = false;

        QFileInfo original = _fileInfosList.value(row);
        if( ! original.exists()) {
            itemText = "<Original file not found>";
        } else if( ! bpid.isEmpty()) {
            QString newBaseName = _filenameFormatter.format(_tracksInformations[bpid]);
            // TODO: Allow user to define a replacement map to choose which char must replace forbid one
            Utils::osFilenameSanitize(newBaseName);
            Utils::simplifySpaces(newBaseName);

            if(newBaseName == original.completeBaseName()) {
                itemText = "<File already have the good name>";
            } else {
                itemText = newBaseName + '.' + original.suffix();
                shouldBeUsed = true;
            }
        } else {
            itemText = "<This file has no track attached>";
        }
        QTableWidgetItem *item = new QTableWidgetItem(itemText);
        item->setData(Qt::UserRole, shouldBeUsed);
        ui->previewTable->setItem(row, TargetFileName, item);
    }
}

void RenameWizard::printEndText() const
{
    ui->outputConsole->appendPlainText(tr("Finished successfully"));
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

    if(index == 3) { // Custom
        ui->pattern->setReadOnly(false);
        connect(ui->pattern, &QLineEdit::textEdited, this, &RenameWizard::updateRenamePreview);
        _patternHelperButton->show();
    } else {
        ui->pattern->setReadOnly(true);
        _patternHelperButton->hide();
        disconnect(ui->pattern, &QLineEdit::textEdited, this, &RenameWizard::updateRenamePreview);
    }
}

void RenameWizard::initializePage(int id)
{
    if(id == ResultPage) {
        // Rename map is built only now, because user can edit target file name manually
        QList<QPair<QFileInfo, QString> > renameList;

        for(int row = 0 ; row < ui->previewTable->rowCount() ; ++row) {
            QTableWidgetItem *item = ui->previewTable->item(row, TargetFileName);
            if(item->data(Qt::UserRole).toBool()) {
                renameList << QPair<QFileInfo, QString>(_fileInfosList.value(row), item->text());
            }
        }

        RenameTask *task = new RenameTask(renameList);
        connect(task, &RenameTask::finished, this, &RenameWizard::printEndText);

        QThreadPool::globalInstance()->start(task);
    }
}
