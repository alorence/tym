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

#include "searchwizard.h"
#include "ui_searchwizard.h"

#include "concretetasks/searchtask.h"

SearchWizard::SearchWizard(QList<QSqlRecord> selectedRecords, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::SearchWizard),
    _selectedRecords(selectedRecords)
{
    ui->setupUi(this);

    connect(ui->searchFromId, SIGNAL(toggled(bool)), this, SLOT(idSearchSelected(bool)));
    connect(ui->searchFromArtistTitle, SIGNAL(toggled(bool)), this, SLOT(titleArtistSearchSelected(bool)));
    connect(ui->customSearch, SIGNAL(toggled(bool)), this, SLOT(customSearchSelected(bool)));

    ui->searchFromId->setChecked(true);
}

SearchWizard::~SearchWizard()
{
    delete ui;
}

QString SearchWizard::pattern() const
{
    return ui->pattern->text();
}

SearchWizard::SearchType SearchWizard::searchType() const
{
    return type;
}

void SearchWizard::setPattern(QString value)
{
    ui->pattern->setText(value);
}

void SearchWizard::idSearchSelected(bool checked)
{
    if(checked ){
        setPattern("%ID%_%OTHER%.%EXT%");
        type = FromId;
    }
}

void SearchWizard::titleArtistSearchSelected(bool checked)
{
    if(checked) {
        setPattern("%ARTISTS% - %TITLE%.%EXT%");
        type = FromArtistTitle;
    }
}

void SearchWizard::customSearchSelected(bool checked)
{
    if(checked) {
        type = Custom;
    }
}

void SearchWizard::initializePage(int id)

{
    if(id == ResultPage) {
        QThread *thread = new QThread(this);
        SearchTask * task = new SearchTask(ui->pattern->text(), type, _selectedRecords);
        task->moveToThread(thread);
        connect(thread, SIGNAL(started()), task, SLOT(run()));
        connect(thread, SIGNAL(finished()), task, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();
    }
}

void SearchWizard::searchThreadFinished()
{
    SearchThread *task = static_cast<SearchThread*>(sender());
    task->deleteLater();
}

