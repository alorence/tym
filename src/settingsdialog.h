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
    
private slots:
    void on_buttons_accepted();
    void on_buttons_rejected();

    void changeDisplayedStack();
private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
