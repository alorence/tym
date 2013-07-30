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
    _patternHelperButton(new PatternButton(FileBasenameParser(), this)),
    _thread(nullptr)
{
    ui->setupUi(this);

    _widgetAppender = new WidgetAppender(ui->outputConsole);
    _widgetAppender->setFormat("%{message}\n");
    _widgetAppender->setDetailsLevel(Logger::Info);
    logger->registerAppender(_widgetAppender);

    connect(ui->pattern1, &QRadioButton::toggled, this, &SearchWizard::updateSearchPattern);
    connect(ui->customSearch, &QRadioButton::toggled, this, &SearchWizard::customSearchSelected);

    ui->pattern->setReadOnly(true);

    ui->patternHorizLayout->addWidget(_patternHelperButton);
    _patternHelperButton->hide();
    connect(_patternHelperButton, &PatternButton::patternSelected, this, &SearchWizard::insertPatternText);

    setStartId(_selectedRecords.count() > 1 ? AutoOptionsPage : SearchTypePage);

    ui->progressBar->hide();
}

SearchWizard::~SearchWizard()
{
    delete ui;
    logger->unRegisterAppender(_widgetAppender);
    delete _widgetAppender;
    delete _patternHelperButton;
}

void SearchWizard::initializePage(int id)
{
    if(id == SearchTypePage) {

        ui->manualSearch->setChecked(true);

    } else if(id == AutoOptionsPage) {

        ui->pattern1->setChecked(true);
        // Ensure we will not try a manual search (in case user click on previous and change the search type)
        ui->searchTerms->clear();

    } else if(id == ResultPage) {

        _thread = new QThread();
        QStringList searchTerms;
        if( ! ui->searchTerms->text().isEmpty()) {
            searchTerms << ui->searchTerms->text();
        }
        SearchTask * task = new SearchTask(_selectedRecords, ui->pattern->text(), searchTerms);
        task->moveToThread(_thread);

        connect(task, &SearchTask::initializeProgression, ui->progressBar, &QProgressBar::setMaximum);
        connect(task, &SearchTask::initializeProgression, ui->progressBar, &QProgressBar::show);
        connect(task, &SearchTask::notifyProgression, ui->progressBar, &QProgressBar::setValue);

        connect(_thread, &QThread::started, task, &SearchTask::run);
        connect(task, &SearchTask::finished, this, &SearchWizard::printEndText);
        connect(task, &SearchTask::finished, _thread, &QThread::quit);

        connect(_thread, &QThread::finished, task, &SearchWizard::deleteLater);
        connect(_thread, &QThread::finished, _thread, &QThread::deleteLater);

        _thread->start();
    }
}

int SearchWizard::nextId() const
{
    switch (currentId()) {
    case AutoOptionsPage:
    case ManualOptionsPage:
        return ResultPage;
    case SearchTypePage:
        return ui->autoSearch->isChecked() ? AutoOptionsPage : ManualOptionsPage;
    default:
        return -1;
    }
}

QString SearchWizard::pattern() const
{
    return ui->pattern->text();
}

void SearchWizard::setPattern(QString value)
{
    ui->pattern->setText(value);
}

void SearchWizard::customSearchSelected(bool checked)
{
    if(checked) {
        _patternHelperButton->show();
        ui->pattern->setReadOnly(false);
    } else {
        _patternHelperButton->hide();
        ui->pattern->setReadOnly(true);
    }
}

void SearchWizard::updateSearchPattern(bool selected)
{
    if(selected) {
        QRadioButton *radio = static_cast<QRadioButton*>(sender());
        ui->pattern->setText(radio->text());
    }
}

void SearchWizard::insertPatternText(const QString & patternText)
{
    ui->pattern->insert(patternText);
}

void SearchWizard::printEndText()
{
    ui->outputConsole->appendPlainText(tr("Finished successfully"));
}

void SearchWizard::initializeProgressBar(int value)
{
    if(ui->progressBar->isHidden()) {
        ui->progressBar->show();
    }

    ui->progressBar->setMaximum(value);
}

