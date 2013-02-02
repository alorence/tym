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
    explicit WidgetChangesObserver(const QString &settingsKey, const QVariant &defaultValue, QObject *parent = 0);

    /*!
     * \brief Initialize the wiget's internal state.
     * Get the initial widget value in QSettings, and use _defaultValue if it doeas not exists.
     */
    void init();

    /*!
     * \brief Save the value displayed by the widget in the QSettings.
     * This method reset the state of the observer. After a call to \a commit(),
     * \a widgetValueChanged() will return false.
     */
    void commit();

protected:
    /*!
     * \brief Return a boolean value representing the current state of the widget.
     * If it state has beem modified and is different from its stae at the initialisation,
     * return true. Otherwise return false.
     */
    bool widgetValueChanged() const;
    /*!
     * \brief Return the value currently displayed by the widget, according to his type.
     * \return A \l QVariant containing the value
     */
    virtual QVariant getWidgetValue() const = 0;
    /*!
     * \brief Modify the state of the widget with the value in parameter.
     * \param value
     */
    virtual void setWidgetValue(const QVariant &value) = 0;

    QString _settingsKey;
    QVariant _registeredValue;
    QVariant _defaultValue;
};

class LineEditChangesObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    LineEditChangesObserver(const QString &settingsKey, QLineEdit * lineEdit, const QVariant &defaultValue, QObject *parent = 0);
    QVariant getWidgetValue() const;
    void setWidgetValue(const QVariant &value);

private:
    QLineEdit * _widget;
};

class CheckBoxChangesObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    CheckBoxChangesObserver(const QString &settingsKey, QCheckBox *checkbox, const QVariant &defaultValue, QObject *parent = 0);
    QVariant getWidgetValue() const;
    void setWidgetValue(const QVariant &value);

private:
    QCheckBox * _widget;
};

class CheckableGroupBoxObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    CheckableGroupBoxObserver(const QString &settingsKey, QGroupBox *groupbox, const QVariant &defaultValue, QObject *parent = 0);
    QVariant getWidgetValue() const;
    void setWidgetValue(const QVariant &value);

private:
    QGroupBox * _widget;
};

#endif // WIDGETCHANGESOBSERVER_H
