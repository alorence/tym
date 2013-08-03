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

#ifndef RENAMECONFIGURATOR_H
#define RENAMECONFIGURATOR_H

#include <QDialog>
#include <QtSql>

#include "tools/patterntool.h"
#include "widgets/patternbutton.h"
#include "tools/utils.h"

namespace Ui {
class RenameConfigurator;
}

class Task;

class RenameConfigurator : public QDialog
{
    Q_OBJECT
    
public:
    explicit RenameConfigurator(const QList<QSqlRecord> &records,
                                QWidget *parent = 0);
    ~RenameConfigurator();

    void showEvent(QShowEvent *) override;

    /*!
     * \brief Build and return the RenameTask this dialog configure.
     * \return
     */
    Task *task() const;

private slots:
    /*!
     * \brief Update ui->pattern value according to entry selected in
     * the pattern combo box
     * \param comboBoxIndex
     */
    void updatePattern(int comboBoxIndex);
    /*!
     * \brief Update target names in the preview table
     * \param pattern Value of ui->pattern->text()
     */
    void updateTargetNames(const QString &pattern);
    /*!
     * \brief Update message and status associated with each row in the preview
     * table. This mainly set the status to ok or identical target name.
     * \param row
     * \param col
     */
    void updateCellInfos(int row, int col);
    /*!
     * \brief Update details* labels with information retrieved from item
     * at given row and col.
     * \param row
     * \param col
     */
    void updateDetails(int row, int col);

private:
    enum Columns {
        OrgnlNameCol,
        TrgtNameCol
    };

    enum UserRoles {
        Bpid = Qt::UserRole + 1,
        Suffix,
        Exists,
        StatusType,
        Message
    };

    Ui::RenameConfigurator *ui;
    FileBasenameFormatter _formatter;
    PatternButton * _patternHelperButton;

    QList<QFileInfo> _fileInfoList;
    QMap<QString, QSqlRecord> _tracksFullInfos;

    QString _customPattern;
    bool _currentIsCustom;
};


#endif // RENAMECONFIGURATOR_H
