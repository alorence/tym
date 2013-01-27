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

#ifndef LIBRARYSTATUSUPDATER_H
#define LIBRARYSTATUSUPDATER_H

#include "task.h"

class BPDatabase;

/*!
 * \brief Define a task to update some Library informations.
 */
class LibraryStatusUpdater : public Task
{
    Q_OBJECT

public:
    /*!
     * \brief Create the task
     * \param parent
     */
    explicit LibraryStatusUpdater(QObject *parent = 0);
    /*!
     * Destroy the task and all its members
     */
    ~LibraryStatusUpdater();

public slots:
    /*!
     * \brief Check and update Library entries if needed.
     *
     * Check all files in Library SQLite table, and update "flags" column
     * if some tracks can't be found on the disk at the path specified.
     */
    void run();

private:
    BPDatabase* _dbHelper;
};

#endif // LIBRARYSTATUSUPDATER_H
