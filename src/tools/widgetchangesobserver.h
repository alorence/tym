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

/*!
 * \brief Base class for monitoring changes performed by user on some widgets' state
 */
class WidgetChangesObserver : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Initialize members. Can't be called directly, since the class is abstract
     * \param settingsKey \link QSettings key for the value monitored
     * \param defaultValue Default value of the widget
     * \param parent
     */
    explicit WidgetChangesObserver(const QString &settingsKey, const QVariant &defaultValue, QObject *parent = 0);

    /*!
     * \brief Initialize the widget's internal state.
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
     * If it state has beem modified and is different from its state at the initialization,
     * return true. Otherwise return false.
     */
    bool widgetValueChanged() const;
    /*!
     * \brief Return the value currently displayed by the widget, according to his type.
     * \return A \link QVariant containing the value
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

/*!
 * \brief Monitor changes performed on a \link QLineEdit text
 */
class LineEditChangesObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    /*!
     * \copybrief WidgetChangesObserver::WidgetChangesObserver
     * This observer manage a \link QLineEdit
     */
    LineEditChangesObserver(const QString &settingsKey, QLineEdit * lineEdit, const QVariant &defaultValue, QObject *parent = 0);
    /*!
     * \copydoc WidgetChangesObserver::getWidgetValue
     */
    QVariant getWidgetValue() const override;
    /*!
     * \copydoc WidgetChangesObserver::setWidgetValue
     */
    void setWidgetValue(const QVariant &value) override;

private:
    QLineEdit * _widget;
};

/*!
 * \brief Monitor changes performed on a \link QCheckBox
 */
class CheckBoxChangesObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    /*!
     * \copybrief WidgetChangesObserver::WidgetChangesObserver
     * This observer manage a \link QCheckBox
     */
    CheckBoxChangesObserver(const QString &settingsKey, QCheckBox *checkbox, const QVariant &defaultValue, QObject *parent = 0);
    /*!
     * \copydoc WidgetChangesObserver::getWidgetValue
     */
    QVariant getWidgetValue() const override;
    /*!
     * \copydoc WidgetChangesObserver::setWidgetValue
     */
    void setWidgetValue(const QVariant &value) override;

private:
    QCheckBox * _widget;
};

/*!
 * \brief Monitor changes performed on a chackable \link QGroupBox
 */
class CheckableGroupBoxObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    /*!
     * \copybrief WidgetChangesObserver::WidgetChangesObserver
     * This observer manage a chackable \link QGroupBox
     */
    CheckableGroupBoxObserver(const QString &settingsKey, QGroupBox *groupbox, const QVariant &defaultValue, QObject *parent = 0);
    /*!
     * \copydoc WidgetChangesObserver::getWidgetValue
     */
    QVariant getWidgetValue() const override;
    /*!
     * \copydoc WidgetChangesObserver::setWidgetValue
     */
    void setWidgetValue(const QVariant &value) override;

private:
    QGroupBox * _widget;
};


class RadioButtonListObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    /*!
     * \copybrief WidgetChangesObserver::WidgetChangesObserver
     * This observer manage a list of \l QRadioButton
     */
    RadioButtonListObserver(const QString &settingsKey, QList<QRadioButton*> radioButtonList, const QVariant &defaultValue, QObject *parent = 0);
    /*!
     * \copydoc WidgetChangesObserver::getWidgetValue
     */
    QVariant getWidgetValue() const override;
    /*!
     * \copydoc WidgetChangesObserver::setWidgetValue
     */
    void setWidgetValue(const QVariant &value) override;

private:
    QList<QRadioButton*> _widgets;
};

class ComboBoxObserver : public WidgetChangesObserver
{
    Q_OBJECT
public:
    /*!
     * \copybrief WidgetChangesObserver::WidgetChangesObserver
     * This observer manage a \l QComboBox
     */
    ComboBoxObserver(const QString &settingsKey, QComboBox *comboBox, const QVariant &defaultValue, QObject *parent = 0);
    /*!
     * \copydoc WidgetChangesObserver::getWidgetValue
     */
    QVariant getWidgetValue() const override;
    /*!
     * \copydoc WidgetChangesObserver::setWidgetValue
     */
    void setWidgetValue(const QVariant &value) override;
private:
    QComboBox *_widget;
};

#endif // WIDGETCHANGESOBSERVER_H
