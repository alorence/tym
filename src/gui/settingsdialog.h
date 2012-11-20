/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag Your Music) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag Your Music) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag Your Music).  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QtCore>
#include <QtGui>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingsDialog(QWidget *parent = 0);
    ~SettingsDialog();

    QVariant getSettingsValue(const QString &key) const;
    
private slots:
    void on_buttons_accepted();
    void on_buttons_rejected();

    void changeDisplayedStack();

private:
    void initMenu();
    void initDefaultValues();
    void initWidgetValues();

    Ui::SettingsDialog *ui;
    QMap<QString, QWidget*> pages;
    QMap<QString, QVariant> settingsValues;
    QMap<QString, QVariant> defaultValues;
};

#endif // SETTINGSDIALOG_H
