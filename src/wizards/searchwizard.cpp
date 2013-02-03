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

#include "searchwizard.h"
#include "ui_searchwizard.h"

#include <Logger.h>
#include <WidgetAppender.h>

#include "concretetasks/searchtask.h"
#include "tools/patterntool.h"

SearchWizard::SearchWizard(QList<QSqlRecord> selectedRecords, QWidget *parent) :
    QWizard(parent),
    ui(new Ui::SearchWizard),
    _selectedRecords(selectedRecords),
    _patternHelperButton(new PatternButton(FileBasenameParser(), this))
{
    ui->setupUi(this);

    _widgetAppender = new WidgetAppender(ui->outputConsole);
    _widgetAppender->setFormat("%m\n");
    Logger::registerAppender(_widgetAppender);

    connect(ui->searchFromId, SIGNAL(toggled(bool)), this, SLOT(idSearchSelected(bool)));
    connect(ui->searchFromArtistTitle, SIGNAL(toggled(bool)), this, SLOT(titleArtistSearchSelected(bool)));
    connect(ui->customSearch, SIGNAL(toggled(bool)), this, SLOT(customSearchSelected(bool)));

    ui->pattern->setReadOnly(true);
    ui->searchFromId->setChecked(true);

    ui->patternHorizLayout->addWidget(_patternHelperButton);
    _patternHelperButton->hide();
    connect(_patternHelperButton, SIGNAL(patternSelected(QString)), this, SLOT(insertPatternText(QString)));
}

SearchWizard::~SearchWizard()
{
    delete ui;
    Logger::unRegisterAppender(_widgetAppender);
    delete _widgetAppender;
    delete _patternHelperButton;
}

QString SearchWizard::pattern() const
{
    return ui->pattern->text();
}

SearchWizard::SearchType SearchWizard::searchType() const
{
    return _type;
}

void SearchWizard::setPattern(QString value)
{
    ui->pattern->setText(value);
}

void SearchWizard::idSearchSelected(bool checked)
{
    if(checked ){
        setPattern("%ID%_%OTHER%");
        _type = FromId;
    }
}

void SearchWizard::titleArtistSearchSelected(bool checked)
{
    if(checked) {
        setPattern("%ARTISTS% - %TITLE%");
        _type = FromArtistTitle;
    }
}

void SearchWizard::customSearchSelected(bool checked)
{
    if(checked) {
        _type = Custom;
        _patternHelperButton->show();
        ui->pattern->setReadOnly(false);
    } else {
        _patternHelperButton->hide();
        ui->pattern->setReadOnly(true);
    }
}

void SearchWizard::insertPatternText(const QString & patternText)
{
    ui->pattern->insert(patternText);
}

void SearchWizard::initializePage(int id)
{
    if(id == ResultPage) {

        QThread *thread = new QThread(this);
        SearchTask * task = new SearchTask(ui->pattern->text(), _type, _selectedRecords);
        task->moveToThread(thread);

        connect(thread, SIGNAL(started()), task, SLOT(run()));
        connect(task, SIGNAL(finished()), thread, SLOT(quit()));

        connect(thread, SIGNAL(finished()), task, SLOT(deleteLater()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

        thread->start();
    }
}

