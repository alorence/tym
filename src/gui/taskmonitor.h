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

#ifndef RESULTDIALOG_H
#define RESULTDIALOG_H

#include <QDialog>
#include "concretetasks/task.h"

namespace Ui {
class TaskMonitor;
}

class TaskMonitor : public QDialog
{
    Q_OBJECT

public:
    explicit TaskMonitor(Task * task, QWidget *parent = 0);
    ~TaskMonitor();

    enum EventType {
        Info,
        Warning,
        Error,
        Success
    };

public slots:
    void updateCurrentState(const QString &state);
    void logEvent(const QString &key, EventType type, const QString &msg);

private slots:
    void toggleLogTree(bool checked);

private:
    Ui::TaskMonitor *ui;

    Task * _task;
};

#endif // RESULTDIALOG_H
