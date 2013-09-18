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

#include "searchconfigurator.h"
#include "ui_searchconfigurator.h"

#include "searchtask.h"

SearchConfigurator::SearchConfigurator(const QList<QSqlRecord> &records,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SearchConfigurator),
    _records(records),
    _currentIsCustom(false)
{
    ui->setupUi(this);

    // Configure manual search table widget
    QStringList headers;
    headers << tr("Track") << tr("Search terms");
    ui->manualSearchTable->setHorizontalHeaderLabels(headers);
    ui->manualSearchTable->setRowCount(records.count());
    // Disable default TAB navigation, we will customize it after
    ui->manualSearchTable->setTabKeyNavigation(false);

    QLineEdit *previousLineEdit = nullptr;
    for(int row = 0 ; row < ui->manualSearchTable->rowCount() ; ++row) {
        QFileInfo fileInfo(records[row].value(Library::FilePath).toString());

        // First cell is a classic widget
        QTableWidgetItem *label = new QTableWidgetItem(fileInfo.fileName());
        // It must not be editable
        label->setFlags(label->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
        ui->manualSearchTable->setItem(row, File, label);

        // Second cell is a QLineEdit
        QLineEdit *lineEdit = new QLineEdit;
        ui->manualSearchTable->setCellWidget(row, SearchTerms, lineEdit);

        // Configure the next lineEdit focused when TAB key is used
        if (previousLineEdit != nullptr) {
            ui->manualSearchTable->setTabOrder(previousLineEdit, lineEdit);
            previousLineEdit = lineEdit;
        }
    }
}

SearchConfigurator::~SearchConfigurator()
{
    delete ui;
}

Task *SearchConfigurator::task() const
{
    SearchTask * task = new SearchTask(_records);
    task->setSearchFromId(ui->bpidSearch->isChecked());
    task->setNaiveSearch(ui->simpleSearch->isChecked());
    task->enableBetterResultDetection(ui->detectBetterResult->isChecked());
    if(ui->manualSearch->isChecked()) {
        QMap<QString, QString> searchMap;
        for(int row = 0 ; row < ui->manualSearchTable->rowCount() ; ++row) {
            searchMap.insert(
                _records[row].value(Library::Uid).toString(),
                ((QLineEdit*) ui->manualSearchTable->cellWidget(row, SearchTerms))->text());
        }
        task->setManualSearch(true, searchMap);
    }

    return task;
}
