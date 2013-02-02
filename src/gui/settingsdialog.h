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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtCore>
#include <QtWidgets>

#include "tools/widgetchangesobserver.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

protected:
    /*!
     * \brief Initialize all registered widgets to the right state.
     * Try to load QSettings value, or the default one if widget state has never been modified.
     * \sa WidgetChangesObserver::init()
     */
    void showEvent(QShowEvent *);
    
private slots:
    /*!
     * \brief Load the setting page corresponding to the current selected item in the menu
     */
    void changeDisplayedStack();

    /*!
     * \brief Save the new widget's state into QSettings, if necessary, and close the dialog.
     */
    void on_buttons_accepted();

private:
    /*!
     * \brief Initialize the menu entries, and link them to their corresponding setting page.
     */
    void initMenu();

    Ui::SettingsDialog *ui;
    QMap<QTreeWidgetItem*, QWidget*> _menuPagesMap;

    QSet<WidgetChangesObserver*> _widgetObservers;
};

#endif // SETTINGSDIALOG_H
