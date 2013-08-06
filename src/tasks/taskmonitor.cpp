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
    _task(task)
{
    ui->setupUi(this);

    connect(_task, &Task::currentStatusChanged,
            this, &TaskMonitor::updateCurrentStatus);
    connect(_task, &Task::finished, [=](){
        ui->progressBar->setValue(ui->progressBar->maximum());
    });

    if(_task->isLongTask()) {
        connect(_task, &Task::notifyProgression, ui->progressBar, &QProgressBar::setValue);
    } else {
        ui->progressBar->hide();
    }
    if(_task->hasMultiResults()) {
        connect(_task, &Task::notifyNewTaskEntity,
                this, &TaskMonitor::initResultElement);
        connect(_task, &Task::newTaskEntityResult,
                this, &TaskMonitor::appendResult);
    } else {
        ui->resultTree->hide();
    }

}

TaskMonitor::~TaskMonitor()
{
    delete ui;
}

void TaskMonitor::updateCurrentStatus(const QString &state)
{

}

void TaskMonitor::initResultElement(const QString &key, const QString &label)
{

}

void TaskMonitor::appendResult(const QString &key, Utils::StatusType type, const QString &msg)
{

}
