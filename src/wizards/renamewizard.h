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

#ifndef RENAMEWIZARD_H
#define RENAMEWIZARD_H

#include <QtCore>
#include <QtWidgets>
#include <QtSql>

#include "commons.h"
#include "tools/patterntool.h"
#include "dbaccess/bpdatabase.h"
#include "concretetasks/renamethread.h"

namespace Ui {
class RenameWizard;
}

class RenameWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit RenameWizard(QList<QPair<int, QSqlRecord> > selected, QWidget *parent = 0);
    ~RenameWizard();

protected:
    void initializePage(int id);
    
private slots:
    void updateRenamePreview();
    void on_patternSelection_currentIndexChanged(int index);
    void renameThreadFinished();

private:
    Ui::RenameWizard *ui;
    QMap<QString, QSqlRecord> tracksInformations;

    enum WizardPages {
        PreviewPage = 0,
        ResultPage
    };

    enum PreviewColumns {
        Bpid = 0,
        Directory,
        OrigFileName,
        TargetFileName
    };
};

#endif // RENAMEWIZARD_H
