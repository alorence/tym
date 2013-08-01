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

class RenameConfigurator : public QDialog
{
    Q_OBJECT
    
public:
    explicit RenameConfigurator(const QList<QSqlRecord> &records,
                                QWidget *parent = 0);
    ~RenameConfigurator();

    QList<QPair<QFileInfo, QString>> renameMap() const;

private slots:
    void updatePattern(int comboBoxIndex);
    void updateTargetNames(const QString &pattern);
    void fillRenameMap();

private:
    enum Columns {
        OriginalNameCol,
        TargetNameCol
    };

    Ui::RenameConfigurator *ui;

    QList<QSqlRecord> _libraryRecords;
    QMap<QString, QSqlRecord> _tracksFullInfos;

    FileBasenameFormatter _filenameFormatter;
    PatternButton * _patternHelperButton;

    QString _customPattern;
    bool _currentIsCustom;

    QList<QPair<QFileInfo, QString>> _renameMap;
    QMap<int, QList<QPair<Utils::StatusType, QString>>> _issues;
};


#endif // RENAMECONFIGURATOR_H
