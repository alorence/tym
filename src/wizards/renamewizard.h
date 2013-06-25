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

#ifndef RENAMEWIZARD_H
#define RENAMEWIZARD_H

#include <QtCore>
#include <QtWidgets>
#include <QtSql>

#include "widgets/patternbutton.h"
#include "tools/patterntool.h"

namespace Ui {
class RenameWizard;
}

class WidgetAppender;

class RenameWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit RenameWizard(QList<QSqlRecord> selected, QWidget *parent = 0);
    ~RenameWizard();

protected:
    void initializePage(int id) override;
    
private slots:
    void updateRenamePreview();
    void insertPatternText(const QString&);
    void printEndText() const;
    void on_patternSelection_currentIndexChanged(int index);

private:
    Ui::RenameWizard* ui;

    FileBasenameFormatter _filenameFormatter;
    PatternButton * _patternHelperButton;

    QMap<QString, QSqlRecord> _tracksInformations;
    QMap<int, QFileInfo> _fileInfosList;

    enum WizardPages {
        PreviewPage = 0,
        ResultPage
    };

    enum PreviewColumns {
        OrigFileName,
        TargetFileName
    };
    WidgetAppender* _widgetAppender;
};

#endif // RENAMEWIZARD_H
