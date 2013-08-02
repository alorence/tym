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
    ui->patternSelection->addItem(tr("Custom"));
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

        // Fill the bpids list
        QString bpid = record.value(Library::Bpid).toString();
        if(!bpid.isEmpty()) {
            bpids << bpid;
        }

        // Cretate cells for the current row
        QTableWidgetItem * origItem = new QTableWidgetItem(orig.fileName());
        QTableWidgetItem * trgtItem = new QTableWidgetItem("");
        // By default, cells can't be edited
        origItem->setFlags(origItem->flags() & ~Qt::ItemIsEditable);
        trgtItem->setFlags(origItem->flags() & ~Qt::ItemIsEditable);

        // Store various information in first cell as UserData
        // It will be used when updating target names on second cells
        origItem->setData(Bpid, bpid);
        origItem->setData(Suffix, orig.suffix());
        origItem->setData(Exists, orig.exists());

        // Configure status that will not change later
        if(!orig.exists()) {
            origItem->setIcon(Utils::instance()->iconForStatusType(Utils::Error));
            origItem->setData(StatusType, Utils::Error);
            origItem->setData(Message, tr("This file does not exists on the "
                                          "disk. It will not be renamed."));
        }
        // Check if the library entry has no information attached
        else if(bpid.isEmpty()) {
            origItem->setIcon(Utils::instance()->iconForStatusType(Utils::Error));
            origItem->setData(StatusType, Utils::Error);
            origItem->setData(Message, tr("This file has no information "
                "attached. It is impossible to generate a new filename. "
                "Please use the search function to retrieve information "
                                          "on this track"));
        } else {
            // Target filename will be editable
            trgtItem->setFlags(origItem->flags() | Qt::ItemIsEditable);
        }

        // Insert created cells into preview table
        ui->previewTable->setItem(row, OrgnlNameCol, origItem);
        ui->previewTable->setItem(row, TrgtNameCol, trgtItem);

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

    // When a cell is modified, update related message/picture
    connect(ui->previewTable, &QTableWidget::cellChanged,
            this, &RenameConfigurator::updateCellInfos);
    // Update details according the selection
    connect(ui->previewTable, &QTableWidget::currentCellChanged,
            this, &RenameConfigurator::updateDetails);
    // Select the first row
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

void RenameConfigurator::showEvent(QShowEvent *)
{
    // Details message will be 2 lines height
    ui->detailsMsg->setMinimumHeight(ui->detailsMsg->height()*2);
}

QList<QPair<QFileInfo, QString>> RenameConfigurator::renameMap()
{
    for(int row = 0 ; row < _renameMap.count() ; ++row) {
        _renameMap[row].second
                = ui->previewTable->item(row, TrgtNameCol)
                ->data(Qt::EditRole).toString();
    }
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

        QTableWidgetItem * origItem = ui->previewTable->item(row, OrgnlNameCol);
        QTableWidgetItem * trgtItem = ui->previewTable->item(row, TrgtNameCol);

        // Beatport ID linked with the library entry
        QString bpid = origItem->data(Bpid).toString();
        // We can't do nothing without bpid defined
        if(bpid.isEmpty()) continue;
        // Suffix of original file (_formatter works on basenames)
        QString suffix = origItem->data(Suffix).toString();

        // We generate the new target name for file
        QString targetBaseName = _formatter.format(_tracksFullInfos[bpid]);
        Utils::instance()->osFilenameSanitize(targetBaseName);
        Utils::instance()->simplifySpaces(targetBaseName);

        // Build the new target name
        QString targetName = targetBaseName + '.' + suffix;

        // Update the second cell in the table with the target name
        trgtItem->setData(Qt::EditRole, targetName);
    }
}

void RenameConfigurator::updateCellInfos(int row, int col)
{
    QTableWidgetItem * origItem = ui->previewTable->item(row, OrgnlNameCol);
    QTableWidgetItem * trgtItem = ui->previewTable->item(row, TrgtNameCol);

    // We will modify an item, avoid infinite loop
    ui->previewTable->blockSignals(true);

    // Status on second cell will be initialized when updating all content
    // of preview table, and when user will manually modify content for a cell
    if(col == TrgtNameCol) {
        // Check if new filename is different than the original one
        if(origItem->text() == trgtItem->data(Qt::EditRole).toString()) {
            origItem->setIcon(Utils::instance()->iconForStatusType(Utils::Info));
            origItem->setData(StatusType, Utils::Info);
            origItem->setData(Message, tr("The target filename is the same "
                                       "than the original one. Nothing to do"));
        } else if (origItem->data(Exists).toBool()) {
            origItem->setIcon(Utils::instance()->iconForStatusType(Utils::Success));
            origItem->setData(StatusType, Utils::Success);
            origItem->setData(Message, tr("The file will be renamed"));
        }

        // Update details fields
        if(ui->previewTable->currentRow() == row)
            updateDetails(row, col);
    }

    // Restore signal system
    ui->previewTable->blockSignals(false);
}


void RenameConfigurator::updateDetails(int row, int)
{
    if(row != -1) {
        QTableWidgetItem *item = ui->previewTable->item(row, OrgnlNameCol);
        ui->detailsMsg->setText(item->data(Message).toString());
        Utils::StatusType s = (Utils::StatusType) item->data(StatusType).toInt();
        ui->detailsPix->setPixmap(Utils::instance()->pixForStatusType(s));
    } else {
        ui->detailsMsg->clear();
        ui->detailsPix->clear();
    }
}
