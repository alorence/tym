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

#include <QSettings>
#include <Logger.h>
#include "widgetchangesobserver.h"

WidgetChangesObserver::WidgetChangesObserver(const QString &settingsKey,
                                             const QVariant &defaultValue, QObject *parent) :
    QObject(parent),
    _settingsKey(settingsKey),
    _defaultValue(defaultValue)
{
}

void WidgetChangesObserver::init()
{
    QSettings settings;
    QVariant newValue = settings.value(_settingsKey, _defaultValue);

    setWidgetValue(newValue);
    _registeredValue = newValue;
}

void WidgetChangesObserver::commit()
{
    if(widgetValueChanged()) {
        QSettings settings;
        settings.setValue(_settingsKey, getWidgetValue());

        _registeredValue = getWidgetValue();
    }
}

bool WidgetChangesObserver::widgetValueChanged() const
{
    return _registeredValue != getWidgetValue();
}

LineEditChangesObserver::LineEditChangesObserver(const QString &settingsKey, QLineEdit *lineEdit,
                                                 const QVariant &defaultValue, QObject *parent) :
    WidgetChangesObserver(settingsKey, defaultValue, parent),
    _widget(lineEdit)
{
}

QVariant LineEditChangesObserver::getWidgetValue() const
{
    return _widget->text();
}

void LineEditChangesObserver::setWidgetValue(const QVariant &value)
{
    if(value.canConvert<QString>()) {
        _widget->setText(value.toString());
    } else {
        LOG_WARNING(tr("Try to assign a non-string value (%1) to the QLineEdit %2")
                    .arg(value.toString()).arg(_widget->objectName()));
    }
}

CheckBoxChangesObserver::CheckBoxChangesObserver(const QString &settingsKey, QCheckBox *checkbox,
                                                 const QVariant &defaultValue, QObject *parent):
    WidgetChangesObserver(settingsKey, defaultValue, parent),
    _widget(checkbox)
{
}

QVariant CheckBoxChangesObserver::getWidgetValue() const
{
    return _widget->isChecked();
}

void CheckBoxChangesObserver::setWidgetValue(const QVariant &value)
{
    if(value.canConvert<bool>()) {
        _widget->setChecked(value.toBool());
    } else {
        LOG_WARNING(tr("Try to assign a non-bool value (%1) to the QCheckbox %2")
                    .arg(value.toString()).arg(_widget->objectName()));
    }
}

CheckableGroupBoxObserver::CheckableGroupBoxObserver(const QString &settingsKey,
        QGroupBox *groupbox, const QVariant &defaultValue, QObject *parent) :
    WidgetChangesObserver(settingsKey, defaultValue, parent),
    _widget(groupbox)
{
    if( ! groupbox->isCheckable()) {
        LOG_WARNING(tr("Try to observe a non-checkable QGroupBox state (%1)")
                    .arg(_widget->objectName()));
    }
}

QVariant CheckableGroupBoxObserver::getWidgetValue() const
{
    return _widget->isChecked();
}

void CheckableGroupBoxObserver::setWidgetValue(const QVariant &value)
{
    if(value.canConvert<bool>()) {
        _widget->setChecked(value.toBool());
    } else {
        LOG_WARNING(tr("Try to assign a non-bool value (%1) to the QGroupBox %2")
                    .arg(value.toString()).arg(_widget->objectName()));
    }
}

RadioButtonListObserver::RadioButtonListObserver(const QString &settingsKey,
        QList<QRadioButton *> radioButtonList, const QVariant &defaultValue, QObject *parent) :
    WidgetChangesObserver(settingsKey, defaultValue, parent),
    _widgets(radioButtonList)
{
}

QVariant RadioButtonListObserver::getWidgetValue() const
{
    for(int i = 0 ; i < _widgets.size() ; ++i) {
        if(_widgets.value(i)->isChecked()) return i;
    }
    return -1;
}

void RadioButtonListObserver::setWidgetValue(const QVariant &value)
{
    _widgets.value(value.toInt())->setChecked(true);
}


ComboBoxObserver::ComboBoxObserver(const QString &settingsKey, QComboBox *comboBox,
                                   const QVariant &defaultValue, QObject *parent) :
    WidgetChangesObserver(settingsKey, defaultValue, parent),
    _widget(comboBox)
{
}

QVariant ComboBoxObserver::getWidgetValue() const
{
    return _widget->itemData(_widget->currentIndex());
}

void ComboBoxObserver::setWidgetValue(const QVariant &value)
{
    _widget->setCurrentIndex(_widget->findData(value));
}
