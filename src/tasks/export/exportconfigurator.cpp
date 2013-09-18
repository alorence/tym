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

#include "exportconfigurator.h"
#include "ui_exportconfigurator.h"

#include <QFileDialog>
#include <QStandardPaths>

#include "exporttask.h"

ExportConfigurator::ExportConfigurator(const QList<QSqlRecord> &libraryRecords,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportConfigurator),
    _libraryRecords(libraryRecords)
{
    ui->setupUi(this);
    connect(ui->browseButton, &QPushButton::clicked, this, &ExportConfigurator::askForTargetFile);

    QSettings settings;
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation)
            + "/tym_generated_collection.nml";
    QString lastPath = settings.value("export/lastOuputFile", defaultPath).toString();

    ui->targetFilePath->setText(lastPath);
}

ExportConfigurator::~ExportConfigurator()
{
    delete ui;
}

Task *ExportConfigurator::task() const
{
    Task *task = new ExportTask(_libraryRecords, ui->targetFilePath->text());
    return task;
}

void ExportConfigurator::askForTargetFile()
{

    QString filePath = QFileDialog::getSaveFileName(this,
                                                    tr("Select file name"),
                                                    ui->targetFilePath->text(),
                                                    "Traktor collection (*.nml)");

    if( ! filePath.isEmpty()) {
        ui->targetFilePath->setText(filePath);
        QSettings settings;
        settings.setValue("export/lastOuputFile", filePath);
    }
}
