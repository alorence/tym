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

#include "about.h"
#include "ui_about.h"

#include <QFile>

#include "config.h"

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);

    ui->aboutText->setText(ui->aboutText->text().replace("%VERSION%", TYM_VERSION));

    QFile licence(":/general/licencetxt");
    licence.open(QIODevice::ReadOnly);

    ui->licenceText->setPlainText(licence.readAll());
}


void About::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        ui->aboutText->setText(ui->aboutText->text().replace("%VERSION%", TYM_VERSION));
    }
    QWidget::changeEvent(e);
}

About::~About()
{
    delete ui;
}
