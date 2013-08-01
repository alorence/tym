#include "renameconfigurator.h"
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

#include "ui_renameconfigurator.h"

#include "dbaccess/librarymodel.h"
#include "tools/utils.h"

RenameConfigurator::RenameConfigurator(const QList<QSqlRecord> &records,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RenameConfigurator),
    _libraryRecords(records),
    _currentIsCustom(false)
{
    ui->setupUi(this);

    // Must be initialized after ui has been initialized
    _patternHelperButton = new PatternButton(_formatter, ui->pattern, this);
    ui->customPatternArea->addWidget(_patternHelperButton);

    // Configure pattern selection comboBox
    ui->patternSelection->addItem("%ARTISTS% - %NAME% (%MIXNAME%)");
    ui->patternSelection->addItem("%ARTISTS% - %TITLE%");
    ui->patternSelection->addItem("%ARTISTS% (%LABEL%) - %TITLE%");
    // "Custom" entry MUST be the last one
    //: Specific value in list of formats, allowing user to create its own
    ui->patternSelection->addItem(tr("Customize"));
    connect(ui->patternSelection, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updatePattern(int)));
    connect(ui->pattern, &QLineEdit::textChanged,
            this, &RenameConfigurator::updateTargetNames);

    // Configure preview table widget
    QStringList headers;
    headers << tr("Original filename") << tr("New filename");
    ui->previewTable->setHorizontalHeaderLabels(headers);
    ui->previewTable->setRowCount(records.count());

    // List of bpids to search, to perform only one db request
    QStringList bpids;

    // Fill first column of preview table, and build the final _renameMap
    for(int row = 0 ; row < ui->previewTable->rowCount() ; ++row) {

        QSqlRecord record = records[row];
        QFileInfo orig(record.value(Library::FilePath).toString());

        // Create first cell, displaying original filename
        QTableWidgetItem * origNameItem = new QTableWidgetItem(orig.fileName());
        // Ensure original filename can't be edited
        origNameItem->setFlags(origNameItem->flags() & ~Qt::ItemIsEditable);
        // origName->setData(Qt::UserRole, bpid);

        // Create second cell, displaying target filename
        QTableWidgetItem * targetNameItem = new QTableWidgetItem("");

        // Add items to the preview table widget
        ui->previewTable->setItem(row, OriginalNameCol, origNameItem);
        ui->previewTable->setItem(row, TargetNameCol, targetNameItem);

        // Fill the bpids list
        QString bpid = record.value(Library::Bpid).toString();
        if(!bpid.isEmpty()) {
            bpids << bpid;
        }

        // Build the final renameMap
        _renameMap << QPair<QFileInfo, QString>(orig, "");
    }

    // Fill the map with all informations about tracks linked to results
    {
        BPDatabase dbHelper("renameWizard");
        QSqlQuery infos = dbHelper.tracksInformations(bpids);
        while(infos.next()) {
            _tracksFullInfos[infos.value(TrackFullInfos::Bpid).toString()]
                    = infos.record();
        }
    }

    // Finally, select the first entry of pattern selection comboBox
    ui->patternSelection->setCurrentIndex(0);
    // Update all other fields and preview table
    updatePattern(0);
}

RenameConfigurator::~RenameConfigurator()
{
    delete ui;
    delete _patternHelperButton;
}

QList<QPair<QFileInfo, QString> > RenameConfigurator::renameMap() const
{
    return _renameMap;
}

void RenameConfigurator::updatePattern(int comboBoxIndex)
{
    // If current selection is "Custom"
    if(comboBoxIndex == ui->patternSelection->count()-1) {
        // Restore the customized pattern if necessary
        if(!_customPattern.isEmpty()) {
            ui->pattern->setText(_customPattern);
        }
        _currentIsCustom = true;
        ui->pattern->setReadOnly(false);
        _patternHelperButton->setVisible(true);
    } else {
        // Save the customized pattern if necessary
        if(_currentIsCustom) {
            _customPattern = ui->pattern->text();
        }
        _currentIsCustom = false;
        ui->pattern->setText(ui->patternSelection->itemText(comboBoxIndex));
        ui->pattern->setReadOnly(true);
        _patternHelperButton->setVisible(false);
    }
}

void RenameConfigurator::updateTargetNames(const QString &pattern)
{
    _formatter.setPattern(pattern);

    for(int row = 0 ; row < ui->previewTable->rowCount() ; ++row) {
        // Original file on disk
        QFileInfo original = _renameMap[row].first;
        // Beatport ID linked with the library entry
        QString bpid = _libraryRecords[row].value(Library::Bpid).toString();
        QString targetName;

        // Check if the original file exists on disk
        if(!original.exists()) {
            QPair<Utils::StatusType, QString> issue;
            issue.first = Utils::Error;
            issue.second = tr("This file does not exists on the disk. "
                              "It will not be renamed.");
            continue;
        }
        // Check if the library entry has no information attached
        else if(bpid.isEmpty()) {
            QPair<Utils::StatusType, QString> issue;
            issue.first = Utils::Error;
            issue.second = tr("This file has no information attached. "
                              "It is impossible to generate a new filename.");
            _issues[row].append(issue);
            continue;
        }
        // New name for the file can be generated
        else {
            QString targetBaseName = _formatter.format(_tracksFullInfos[bpid]);
            Utils::osFilenameSanitize(targetBaseName);
            Utils::simplifySpaces(targetBaseName);

            // Build the new target name
            targetName = targetBaseName + '.' + original.suffix();

            // Update the second cell in the table with the target name
            ui->previewTable->item(row, TargetNameCol)->setText(targetName);
        }

        // Check if new filename is different than the original one
        if(ui->previewTable->item(row, OriginalNameCol)->text() == targetName) {
            QPair<Utils::StatusType, QString> issue;
            issue.first = Utils::Info;
            issue.second = tr("The target filename is the same than the "
                              "original one. Nothing to do");
        }
    }
}

void RenameConfigurator::fillRenameMap()
{
    for(int row = 0 ; row < _renameMap.count() ; ++row) {
        _renameMap[row].second
                = ui->previewTable->item(row, TargetNameCol)->text();
    }
}
