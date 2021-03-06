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

#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QRunnable>

#include <Logger.h>
#include "tools/utils.h"
#include "taskmonitor.h"

//NOTE: Missing doc on some methods
/*!
 * @brief Define an atomic task.
 * This class is an interface to define asynchronous tasks. It extends QRunnable
 * to define run() method, and QObject to implements signals / slots mechanism.
 *
 * Tasks can be launched directly in a QThread (which run a Qt envent loop),
 * via a QThreadPool or directly without thread, depending on how it works and
 * if it needs to launch a event loop or not.
 */
class Task : public QObject, public QRunnable
{
    Q_OBJECT
public:
    /*!
     * @brief Task::Task construct a Task.
     * @param parent
     */
    explicit Task(QObject *parent = 0);

    /*!
     * \brief Informs if that task has multiple subtasks.
     * The default is true, but concrete tasks can set _hasMultiResults or
     * reimplements this method to change this property.
     * \return A boolean value
     */
    virtual bool hasMultiResults() const;

public slots:
    /**
     * @brief Concrete code to execute when task is launched.
     */
    virtual void run() = 0;

signals:
    /*!
     * @brief Signal emitted when all work is done for the task.
     */
    void finished(QString);

    void initializeProgression(int value);
    void notifyProgression(int newValue);

    void currentStatusChanged(QString state);
    void newSubResultElement(QString key, QString label);
    void subResultAvailable(QString key, Utils::StatusType type, QString text);

protected:
    void increaseProgressStep(int step = 1);
    bool _hasMultiResults;

private:
    int _progressValue;
};

#endif // TASK_H
