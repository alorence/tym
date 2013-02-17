/*****************************************************************************
Copyright 2013 Antoine Lorence. All right reserved.

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

#include "exportplaylistwizard.h"
#include "ui_exportplaylistwizard.h"

#include "QFileDialog"

#include "Logger.h"
#include "WidgetAppender.h"

#include "concretetasks/exportplaylisttask.h"
#include "commons.h"

ExportPlaylistWizard::ExportPlaylistWizard(const QList<QSqlRecord> &selected, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::ExportPlaylistWizard),
    _selectedRecords(selected)
{
    ui->setupUi(this);

    // Configure console
    _widgetAppender = new WidgetAppender(ui->outputConsole);
    _widgetAppender->setFormat("%m\n");
    Logger::registerAppender(_widgetAppender);

    connect(ui->multiPlaylist, &QRadioButton::toggled, this, &ExportPlaylistWizard::updateMainOptionElements);

    connect(this, &ExportPlaylistWizard::accepted, this, &ExportPlaylistWizard::saveSelectedOptions);

    ui->selectPathButton->setDefaultAction(ui->actionSelect_playlist_name);
}

ExportPlaylistWizard::~ExportPlaylistWizard()
{
    delete ui;
    Logger::unRegisterAppender(_widgetAppender);
    delete _widgetAppender;
}

void ExportPlaylistWizard::showEvent(QShowEvent *)
{
    if(_settings.value("export/mainOption", MultiPlaylists) == MultiPlaylists) {
        ui->multiPlaylist->setChecked(true);
    } else {
        ui->uniquePlaylist->setChecked(true);
    }
    updateMainOptionElements();
}

void ExportPlaylistWizard::initializePage(int id)
{
    if(id == ResultPage) {

        ui->outputConsole->clear();

        if(ui->multiPlaylist->isChecked()) {

            QMap<QString, QList<QSqlRecord> > exportMap;

            foreach(QSqlRecord record, _selectedRecords) {
                QFileInfo fileInfo(record.value(Library::FilePath).toString());

                if(fileInfo.exists()) {
                    QString outputName = fileInfo.canonicalPath() + "/" + fileInfo.dir().dirName() + ".nml";
                    exportMap[outputName] << record;
                }
            }

            QMapIterator<QString, QList<QSqlRecord> > it(exportMap);
            while(it.hasNext()) {
                QString path = it.next().key();
                QList<QSqlRecord> records = it.value();

                ExportPlaylistTask * task = new ExportPlaylistTask(records, path);
                QThreadPool::globalInstance()->start(task);
            }

        } else {
            QList<QSqlRecord> records;

            foreach(QSqlRecord record, _selectedRecords) {
                QFileInfo fileInfo(record.value(Library::FilePath).toString());

                if(fileInfo.exists()) {
                    records << record;
                }
            }

            ExportPlaylistTask * task = new ExportPlaylistTask(records, ui->outputPath->text());
            connect(task, &ExportPlaylistTask::finished, this, &ExportPlaylistWizard::printEndText);

            QThreadPool::globalInstance()->start(task);
        }
    }
}

void ExportPlaylistWizard::updateMainOptionElements()
{
    if(ui->uniquePlaylist->isChecked()) {
        ui->outputPathSelection->show();
        ui->mainOptionHelp->setText(tr("One playlist will be generated, containing all tracks selected. You have to choose where this playlist must be written."));
        ui->outputPath->setText(_settings.value("export/lastOutputPath", "").toString());
    } else {
        ui->outputPathSelection->hide();
        ui->mainOptionHelp->setText(tr("One playlist will be generated for each folder containing at least one track you selected. Playlist filename will reflect folder name."));
    }
}

void ExportPlaylistWizard::saveSelectedOptions()
{
    if(ui->uniquePlaylist->isChecked()) {
        _settings.setValue("export/mainOption", UniquePlaylist);
    } else {
        _settings.setValue("export/mainOption", MultiPlaylists);
    }
}

void ExportPlaylistWizard::on_actionSelect_playlist_name_triggered()
{
    QString lastImportDir = _settings.value("lastOpenedDir", QStandardPaths::writableLocation(QStandardPaths::MusicLocation)).toString();
    QString dir = _settings.value("export/lastOutputPath", lastImportDir).toString();

    QString file = QFileDialog::getSaveFileName(this, tr("Select playlist filename"), dir, "Traktor playlist (*.nml)");

    if( ! file.isEmpty()) {
        ui->outputPath->setText(file);
        _settings.setValue("export/lastOutputPath", file);
    }
}

void ExportPlaylistWizard::printEndText()
{
    ui->outputConsole->appendPlainText("Finished successfully");
}
