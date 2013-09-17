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

#include <QKeyEvent>

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "tools/langmanager.h"
#include "widgets/patternbutton.h"
#include "tools/patterntool.h"

#include "commons.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog),
    _patternButton(nullptr)
{
    ui->setupUi(this);

    ui->splitter->setStretchFactor(0, 1);
    ui->splitter->setStretchFactor(1, 4);

    // Initialize the menu
    initMenu();
    // Configure actions related to preferred patterns list
    initPatternsConfigurationWidget();

    // Simply close the dialog when "Cancel" is pressed
    // Ok action is connected automatically with on_buttons_accepted()
    connect(ui->buttons, &QDialogButtonBox::rejected,
            this, &SettingsDialog::close);

    /********************************
     * General
     ********************************/
    _widgetObservers << new ListWidgetObserver(TYM_PATH_PATTERNS, ui->patternList,
                                               TYM_DEFAULT_PATTERNS, false, this);

    /********************************
     * Interface
     ********************************/
    QMap<QString, QString> translations = LangManager::instance()->translationsAvailable();
    for(QString lang : translations.keys()) {
        ui->langSelection->addItem(translations[lang], lang);
    }
    _widgetObservers << new ComboBoxObserver(TYM_PATH_LANGUAGE, ui->langSelection,
                                             TYM_DEFAULT_LANGUAGE, this);
    _widgetObservers << new CheckboxObserver(TYM_PATH_DISPLAY_COLORS, ui->libColors,
                                             TYM_DEFAULT_DISPLAY_COLORS, this);
    QList<QRadioButton*> keyStyleValueList;
    keyStyleValueList << ui->bpStyle << ui->mikStyle;
    _widgetObservers << new RadioButtonListObserver(TYM_PATH_KEY_STYLE, keyStyleValueList,
                                                    TYM_DEFAULT_KEY_STYLE, this);
    /********************************
     * Network
     ********************************/
    _widgetObservers << new CheckableGroupBoxObserver(TYM_PATH_PROXY_ENABLED, ui->proxyGroup,
                                                      TYM_DEFAULT_PROXY_ENABLED, this);
    _widgetObservers << new LineEditObserver(TYM_PATH_PROXY_HOST, ui->proxyHost,
                                                    TYM_DEFAULT_PROXY_HOST, this);
    _widgetObservers << new LineEditObserver(TYM_PATH_PROXY_USER, ui->proxyUser,
                                                    TYM_DEFAULT_PROXY_USER, this);
    _widgetObservers << new LineEditObserver(TYM_PATH_PROXY_PWD, ui->proxyPwd,
                                                    TYM_DEFAULT_PROXY_PWD, this);
    _widgetObservers << new LineEditObserver(TYM_PATH_PROXY_PORT, ui->proxyPort,
                                                    TYM_DEFAULT_PROXY_PORT, this);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
    delete _patternButton;
}

void SettingsDialog::showEvent(QShowEvent *)
{
    // All wigets must be initialized each time the settings dialog is opened,
    // not only when it is built (a unique instance is maintened by MainWindow)
    for(WidgetChangesObserver *observer : _widgetObservers) {
        observer->init();
    }
}

void SettingsDialog::changeEvent(QEvent *e)
{
    if(e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(e);
}

void SettingsDialog::keyPressEvent(QKeyEvent *event)
{
    Qt::Key key = (Qt::Key) event->key();
    // Do not propagate KeyEvent when Enter/Return is pressed inside the patternEdit widget
    if( (key == Qt::Key_Return || key == Qt::Key_Enter) && ui->patternEdit->hasFocus()) {
        return;
    }
    QDialog::keyPressEvent(event);
}

void SettingsDialog::changeDisplayedStack()
{
    ui->content->setCurrentWidget(_menuPagesMap[ui->menu->selectedItems().first()]);
}

void SettingsDialog::on_buttons_accepted()
{
    for(WidgetChangesObserver *observer : _widgetObservers) {
        observer->commit();
    }

    accept();
}

void SettingsDialog::initMenu()
{
    connect(ui->menu, SIGNAL(itemSelectionChanged()), SLOT(changeDisplayedStack()));

    QTreeWidgetItem *item;

    item = new QTreeWidgetItem(ui->menu, QStringList()<<tr("General"));
    _menuPagesMap[item] = ui->generalPage;

    item = new QTreeWidgetItem(ui->menu, QStringList()<<tr("Interface"));
    _menuPagesMap[item] = ui->interfacePage;

    item = new QTreeWidgetItem(ui->menu, QStringList()<<tr("Network"));
    _menuPagesMap[item] = ui->networkPage;

    ui->content->setCurrentIndex(0);
}

void SettingsDialog::initPatternsConfigurationWidget()
{
    connect(ui->addButton, &QPushButton::clicked, [this](){
        new QListWidgetItem("", ui->patternList);
    });
    connect(ui->removeButton, &QPushButton::clicked, [this](){
        delete ui->patternList->takeItem(ui->patternList->currentRow());
    });
    // Reset all elements in the list
    connect(ui->resetButton, &QPushButton::clicked, [this](){
        ui->patternList->clear();
        for(QString pattern : QStringList(TYM_DEFAULT_PATTERNS)) {
            new QListWidgetItem(pattern, ui->patternList);
        }
    });
    // Update list item when line edit loose focus or when Enter/Return is pressed (see keyEvent())
    connect(ui->patternEdit, &QLineEdit::editingFinished,
        [this](){
            if(ui->patternList->currentRow() >= 0) {
                ui->patternList->item(ui->patternList->currentRow())->setText(ui->patternEdit->text());
            }
        }
    );
    // Display in line edit text corresponding on list item selected
    connect(ui->patternList, &QListWidget::currentItemChanged,
        [this](QListWidgetItem * current, QListWidgetItem * previous){
            if(current != nullptr) {
                ui->patternEdit->setText(current->text());
            }
        }
    );

    // Initialize the helper button to build patterns and add it to the layout
    FileBasenameFormatter formatter;
    _patternButton = new PatternButton(formatter, ui->patternEdit, this);
    static_cast<QGridLayout*>(ui->patternGroup->layout())->addWidget(_patternButton, 1, 1);
}

