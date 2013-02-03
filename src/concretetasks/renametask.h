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

#ifndef RENAMETHREAD_H
#define RENAMETHREAD_H

#include <QtCore>
#include "task.h"

/*!
 * \brief Rename files according to QMap gived as parameter.
 *
 * \sa RenameTask::run()
 */
class RenameTask : public Task
{
    Q_OBJECT

public:
    /*!
     * \brief Construct a new RenameTask
     * \param renameMap The map defining which track must be renamed with whioch name
     * \param parent
     * \sa RenameTask::run()
     */
    explicit RenameTask(QList<QPair<QFileInfo, QString> > renameMap, QObject *parent = 0);

    /*!
     * \brief Execute the rename task.
     *
     * According to the QMap<QString, QString> used to create the class, rename each @a key
     * into @value and update database with new file names.
     */
    void run();
private:
    QList<QPair<QFileInfo, QString> > _renameMap;
};

#endif // RENAMETHREAD_H
