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

ExportConfigurator::ExportConfigurator(const QList<QSqlRecord> &records,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExportConfigurator),
    _records(records)
{
    ui->setupUi(this);
    connect(ui->browseButton, &QPushButton::clicked, this, &ExportConfigurator::askForTargetFile);
}

ExportConfigurator::~ExportConfigurator()
{
    delete ui;
}

Task *ExportConfigurator::task() const
{
    Task *task = new ExportTask(_records, ui->targetFilePath->text());
    return task;
}

void ExportConfigurator::askForTargetFile()
{
    QSettings settings;
    QString defaultDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    QString lastOpenedDir = settings.value("lastOpenedDir", defaultDir).toString();
    QString targetDir = settings.value("export/lastOutputPath", lastOpenedDir).toString();

    QString filePath = QFileDialog::getSaveFileName(this,
                                tr("Select file name"), targetDir, "Traktor collection (*.nml)");

    if( ! filePath.isEmpty()) {
        ui->targetFilePath->setText(filePath);
        settings.setValue("export/lastOutputPath", filePath);

        QFile target(filePath);
        if(target.exists()) {
            // TODO: inform user that the file will be overwritten
        }
    }
}
