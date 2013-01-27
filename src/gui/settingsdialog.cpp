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

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    initMenu();
    initDefaultValues();
    initWidgetValues();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::initMenu()
{
    connect(ui->menu, SIGNAL(itemSelectionChanged()), SLOT(changeDisplayedStack()));

    QTreeWidgetItem * general = new QTreeWidgetItem(ui->menu, QStringList()<<"General");
    general->setData(0, Qt::UserRole, QVariant("general"));
    pages.insert("general", ui->general);

    QTreeWidgetItem * textIn = new QTreeWidgetItem(general, QStringList()<<"Text input");
    textIn->setData(0, Qt::UserRole, QVariant("textIn"));
    pages.insert("textIn", ui->textIn);

    QTreeWidgetItem * textOut = new QTreeWidgetItem(general, QStringList()<<"Text output");
    textOut->setData(0, Qt::UserRole, QVariant("textOut"));
    pages.insert("textOut", ui->textOut);

    QTreeWidgetItem * network = new QTreeWidgetItem(ui->menu, QStringList()<<"Network");
    network->setData(0, Qt::UserRole, QVariant("network"));
    pages.insert("network", ui->network);

    QTreeWidgetItem * beatport = new QTreeWidgetItem(network, QStringList()<<"Beatport");
    beatport->setData(0, Qt::UserRole, QVariant("beatport"));
    pages.insert("beatport", ui->beatport);
}

void SettingsDialog::initDefaultValues()
{
    defaultValues["settings/network/beatport/apihost"] = QVariant("api.beatport.com");
}

void SettingsDialog::initWidgetValues()
{
    ui->apiHost->setText(getSettingsValue("settings/network/beatport/apihost").toString());
}

QVariant SettingsDialog::getSettingsValue(const QString &key) const
{
    QSettings settings;
    return settings.value(key, defaultValues.value(key));
}

void SettingsDialog::on_buttons_accepted()
{
    QSettings settings;
    settings.setValue("settings/network/beatport/apihost", QVariant(ui->apiHost->text()));

    close();
}

void SettingsDialog::on_buttons_rejected()
{
    close();
}

void SettingsDialog::changeDisplayedStack()
{
    QList<QTreeWidgetItem*> items = ui->menu->selectedItems();
    QString key = items.at(0)->data(0, Qt::UserRole).toString();
    ui->content->setCurrentIndex(ui->content->indexOf(pages[key]));
}

