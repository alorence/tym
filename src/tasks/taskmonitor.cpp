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

#include "taskmonitor.h"
#include "ui_taskmonitor.h"

#include "tasks/task.h"

TaskMonitor::TaskMonitor(Task *task, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskMonitor),
    _task(task),
    _thread(new QThread)
{
    ui->setupUi(this);

    connect(_task, &Task::currentStatusChanged,
            this, &TaskMonitor::updateCurrentStatus);
    connect(_task, &Task::initializeProgression,
            this, &TaskMonitor::initializeProgressBar);

    // By default, progress bar is not shown. If initializeProgression
    // is emitted, show the widget
    ui->progressBar->hide();

    if(_task->hasMultiResults()) {
        connect(_task, &Task::newSubResultElement,
                this, &TaskMonitor::initResultElement);
        connect(_task, &Task::subResultAvailable,
                this, &TaskMonitor::appendResult);
    } else {
        ui->resultTree->hide();
        resize(440, 120);
    }

    _task->moveToThread(_thread);
    connect(_thread, &QThread::started, _task, &Task::run);
    connect(_task, &Task::finished,
            this, &TaskMonitor::finalizeMonitoring);

    ui->finalText->hide();
}

TaskMonitor::~TaskMonitor()
{
    _thread->quit();
    delete ui;
    _thread->wait();
    _thread->deleteLater();
}

void TaskMonitor::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);
    _thread->start();
}

void TaskMonitor::initializeProgressBar(int max)
{
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(0);
    connect(_task, &Task::notifyProgression,
            ui->progressBar, &QProgressBar::setValue);
    ui->progressBar->show();
}

void TaskMonitor::updateCurrentStatus(const QString &state)
{
    ui->statusLabel->setText(state);
}

void TaskMonitor::initResultElement(const QString &key, const QString &label)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << label);
    _resultsItems.insert(key, item);
}

void TaskMonitor::appendResult(const QString &key, Utils::StatusType type, const QString &msg)
{
    QPixmap iconForStatus = Utils::instance()->pixForStatusType(type);

    QTreeWidgetItem *item = _resultsItems[key];
    if(item->treeWidget() == 0) {
        ui->resultTree->addTopLevelItem(item);
    }
    if(item->data(0, Qt::UserRole).toInt() < type) {
        item->setData(0, Qt::UserRole, type);
        item->setData(0, Qt::DecorationRole, iconForStatus);
    }
    QTreeWidgetItem *child = new QTreeWidgetItem(item, QStringList() << msg);
    child->setData(0, Qt::DecorationRole, iconForStatus);

}

void TaskMonitor::finalizeMonitoring(const QString &finalText) {
    if(!ui->progressBar->isHidden()) {
        ui->progressBar->setValue(ui->progressBar->maximum());
    }
    if(!finalText.isEmpty()) {
        ui->finalText->setText(finalText);
        ui->finalText->show();
        ui->statusLabel->hide();
    }
    _thread->quit();
}
