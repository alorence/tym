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

#include "patternbutton.h"

#include <QMenu>
#include <QAction>
#include <QRegularExpression>

#include "tools/patterntool.h"

PatternButton::PatternButton(const PatternTool &patterntool, QWidget *parent) :
    QPushButton(parent)
{
    setText(tr("Pattern"));


    QMenu * menu = new QMenu(this);
    foreach(QString patternString, patterntool.availablesPatterns().keys()) {

        PatternElement pe = patterntool.availablesPatterns()[patternString];

        QString text = QString("%1 (%%2%)").arg(pe.displayName()).arg(patternString);

        QAction *action = new QAction(text, menu);
        action->setIconVisibleInMenu(false);
        action->setStatusTip(pe.description());
        menu->addAction(action);

        _patternElementMap[action] = patternString;
    }
    setMenu(menu);
    connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuActionTriggered(QAction*)));
}

PatternButton::~PatternButton()
{
}


void PatternButton::menuActionTriggered(QAction *action)
{
    QString decoratedPattern = QString("%%1%").arg(_patternElementMap.value(action));
    emit patternSelected(decoratedPattern);
}
