#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
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
    ui->content->setCurrentIndex(ui->content->indexOf(pages[key]));
}
