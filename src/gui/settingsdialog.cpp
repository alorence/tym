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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include "commons.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 4);

    initMenu();

    connect(ui->buttons, &QDialogButtonBox::rejected, this, &SettingsDialog::close);

    /********************************
     * Interface
     ********************************/
    _widgetObservers << new CheckBoxChangesObserver(TYM_PATH_DISPLAY_FULLPATHS,
                                                    ui->libFullPaths, TYM_DEFAULT_DISPLAY_FULLPATHS, this);
    _widgetObservers << new CheckBoxChangesObserver(TYM_PATH_DISPLAY_COLORS,
                                                    ui->libColors, TYM_DEFAULT_DISPLAY_COLORS, this);
    _widgetObservers << new CheckBoxChangesObserver(TYM_PATH_DISPLAY_CHECKBOXES,
                                                    ui->libCheckboxes, TYM_DEFAULT_DISPLAY_CHECKBOXES, this);
    /********************************
     * Network
     ********************************/
    _widgetObservers << new CheckableGroupBoxObserver(TYM_PATH_PROXY_ENABLED,
                                                      ui->proxyGroup, TYM_DEFAULT_PROXY_ENABLED, this);
    _widgetObservers << new LineEditChangesObserver(TYM_PATH_API_URL,
                                                    ui->bpApiHost, TYM_DEFAULT_API_URL, this);
    _widgetObservers << new LineEditChangesObserver(TYM_PATH_PROXY_HOST,
                                                    ui->proxyHost, TYM_DEFAULT_PROXY_HOST, this);
    _widgetObservers << new LineEditChangesObserver(TYM_PATH_PROXY_USER,
                                                    ui->proxyUser, TYM_DEFAULT_PROXY_USER, this);
    _widgetObservers << new LineEditChangesObserver(TYM_PATH_PROXY_PWD,
                                                    ui->proxyPwd, TYM_DEFAULT_PROXY_PWD, this);
    _widgetObservers << new LineEditChangesObserver(TYM_PATH_PROXY_PORT,
                                                    ui->proxyPort, TYM_DEFAULT_PROXY_PORT, this);

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::showEvent(QShowEvent *)
{
    foreach(WidgetChangesObserver *observer, _widgetObservers) {
        observer->init();
    }
}

void SettingsDialog::changeDisplayedStack()
{
    ui->content->setCurrentWidget(_menuPagesMap[ui->menu->selectedItems().first()]);
}

void SettingsDialog::on_buttons_accepted()
{
    foreach(WidgetChangesObserver *observer, _widgetObservers) {
        observer->commit();
    }

    accept();
}

void SettingsDialog::initMenu()
{
    connect(ui->menu, SIGNAL(itemSelectionChanged()), SLOT(changeDisplayedStack()));

    QTreeWidgetItem *item;

    item = new QTreeWidgetItem(ui->menu, QStringList()<<tr("Interface"));
    _menuPagesMap[item] = ui->interfacePage;

    item = new QTreeWidgetItem(ui->menu, QStringList()<<tr("Network"));
    _menuPagesMap[item] = ui->networkPage;

    ui->content->setCurrentWidget(ui->interfacePage);
}

