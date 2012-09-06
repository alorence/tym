#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    connect(ui->menu, SIGNAL(itemSelectionChanged()), SLOT(changeDisplayedStack()));

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_buttons_accepted()
{
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
    ui->content->setCurrentIndex(2);
}
