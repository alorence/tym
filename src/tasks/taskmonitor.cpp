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

    if(_task->isLongTask()) {
        connect(_task, &Task::notifyProgression, ui->progressBar, &QProgressBar::setValue);
    } else {
        ui->progressBar->hide();
    }

    connect(ui->moreButton, &QPushButton::toggled, this, &TaskMonitor::toggleLogTree);

    // TODO: Implements pause in Task with QMutex/QWaitcondition
    ui->pauseButton->hide();
}

TaskMonitor::~TaskMonitor()
{
    delete ui;
}

void TaskMonitor::updateCurrentState(const QString &state)
{

}

void TaskMonitor::logEvent(const QString &key, Utils::StatusType type, const QString &msg)
{

}

void TaskMonitor::toggleLogTree(bool checked)
{
    if(checked) {
        //: Text displayed on the button used to display/hide log tree frame (when it is visible)
        ui->moreButton->setText(tr("Hide details"));
    } else {
        //: Text displayed on the button used to display/hide log tree frame (when it is hidden)
        ui->moreButton->setText(tr("Show details"));
    }
    ui->resultTree->setVisible(checked);
}
