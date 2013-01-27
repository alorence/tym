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

#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QWidget>
#include <QPlainTextEdit>
#include "ui_consolewidget.h"

namespace Ui {
class ConsoleWidget;
}

class ConsoleWidget : public QPlainTextEdit
{
    Q_OBJECT
    
public:
    explicit ConsoleWidget(QWidget *parent = 0):
        QPlainTextEdit(parent),
        ui(new Ui::ConsoleWidget)
    {
        ui->setupUi(this);
    }
    ~ConsoleWidget() {
        delete ui;
    }
    
private:
    Ui::ConsoleWidget *ui;
};

#endif // CONSOLEWIDGET_H
