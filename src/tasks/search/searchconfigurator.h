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

#ifndef SEARCHCONFIGURATOR_H
#define SEARCHCONFIGURATOR_H

#include <QDialog>
#include <QtSql>

#include "interfaces/taskfactory.h"

namespace Ui {
class SearchConfigurator;
}

class Task;

//TODO: Implements a dynamic pattern list, based on user settings

class SearchConfigurator : public QDialog, public TaskFactory
{
    Q_OBJECT

public:
    explicit SearchConfigurator(const QList<QSqlRecord> &records,
                                QWidget *parent = 0);
    ~SearchConfigurator();

    /*!
     * \brief Build and return the SearchTask this dialog configure.
     * \return
     */
    Task *task() const override;

private slots:

private:
    Ui::SearchConfigurator *ui;
};


#endif // SEARCHCONFIGURATOR_H
