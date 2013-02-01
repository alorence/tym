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

#include <QSettings>
#include "widgetchangesobserver.h"

WidgetChangesObserver::WidgetChangesObserver(const QString &settingsKey, QObject *parent) :
    QObject(parent),
    _settingsKey(settingsKey)
{
}

void WidgetChangesObserver::commit()
{
    if(widgetValueChanged()) {
        QSettings settings;
        settings.setValue(_settingsKey, getValue());

        _registeredValue = getValue();
    }
}

bool WidgetChangesObserver::widgetValueChanged() const
{
    return _registeredValue != getValue();
}

LineEditChangesObserver::LineEditChangesObserver(const QString &settingsKey, QLineEdit *lineEdit, QObject *parent) :
    WidgetChangesObserver(settingsKey, parent),
    _widget(lineEdit)
{
    _registeredValue = lineEdit->text();
}

QVariant LineEditChangesObserver::getValue() const
{
    return _widget->text();
}

CheckBoxChangesObserver::CheckBoxChangesObserver(const QString &settingsKey, QCheckBox *checkbox, QObject *parent):
    WidgetChangesObserver(settingsKey, parent),
    _widget(checkbox)
{
    _registeredValue = checkbox->isChecked();
}

QVariant CheckBoxChangesObserver::getValue() const
{
    return _widget->isChecked();
}
