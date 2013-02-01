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

#ifndef WIDGETCHANGESOBSERVER_H
#define WIDGETCHANGESOBSERVER_H

#include <QObject>
#include <QtWidgets>

class WidgetChangesObserver : public QObject
{
    Q_OBJECT
public:
    explicit WidgetChangesObserver(const QString &settingsKey, QObject *parent = 0);

    void commit();
    bool widgetValueChanged() const;
    virtual QVariant getValue() const = 0;

protected:
    QString _settingsKey;
    QVariant _registeredValue;
};

class LineEditChangesObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    LineEditChangesObserver(const QString &settingsKey, QLineEdit * lineEdit, QObject *parent = 0);
    QVariant getValue() const;

private:
    QLineEdit * _widget;
};

class CheckBoxChangesObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    CheckBoxChangesObserver(const QString &settingsKey, QCheckBox *checkbox, QObject *parent = 0);
    QVariant getValue() const;

private:
    QCheckBox * _widget;
};

#endif // WIDGETCHANGESOBSERVER_H
