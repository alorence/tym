#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
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
