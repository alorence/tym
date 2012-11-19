/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag your Library) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag your Library) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag your Library).  If not, see <http://www.gnu.org/licenses/>.
*/

#include "searchwizard.h"
#include "ui_searchwizard.h"

SearchWizard::SearchWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::SearchWizard)
{
    ui->setupUi(this);

    connect(ui->searchFromId, SIGNAL(toggled(bool)), this, SLOT(idSearchSelected(bool)));
    connect(ui->searchFromArtistTitle, SIGNAL(toggled(bool)), this, SLOT(titleArtistSearchSelected(bool)));
    connect(ui->customSearch, SIGNAL(toggled(bool)), this, SLOT(customSearchSelected(bool)));

    ui->searchFromId->setChecked(true);
}

SearchWizard::~SearchWizard()
{
    delete ui;
}

QString SearchWizard::pattern() const
{
    return ui->pattern->text();
}

SearchWizard::SearchType SearchWizard::searchType() const
{
    return type;
}

void SearchWizard::setPattern(QString value)
{
    ui->pattern->setText(value);
}

void SearchWizard::idSearchSelected(bool checked)
{
    if(checked ){
        setPattern("%ID%_%OTHER%.%EXT%");
        type = FromId;
    }
}

void SearchWizard::titleArtistSearchSelected(bool checked)
{
    if(checked) {
        setPattern("%ARTISTS% - %TITLE%.%EXT%");
        type = FromArtistTitle;
    }
}

void SearchWizard::customSearchSelected(bool checked)
{
    if(checked) {
        type = Custom;
    }
}
