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

#ifndef SEARCHWIZARD_H
#define SEARCHWIZARD_H

#include <QWizard>
#include <QtCore>
#include <QSqlRecord>

#include "gui/patternbutton.h"

namespace Ui {
class SearchWizard;
}

class WidgetAppender;

/*!
 * \brief The Wizard used to perform a search.
 *
 *
 */
class SearchWizard : public QWizard
{
    Q_OBJECT
    
public:
    explicit SearchWizard(QList<QSqlRecord> selectedRecords, QWidget *parent = 0);
    ~SearchWizard();
    void initializePage(int id);
    int nextId() const;

    QString pattern() const;

private slots:
    void customSearchSelected(bool);
    void updateSearchPattern(bool);
    void insertPatternText(const QString&);
    void printEndText();

private:
    void setPattern(QString value);
    enum WizardPages {
        SearchTypePage = 0,
        AutoOptionsPage,
        ManualOptionsPage,
        ResultPage
    };
    Ui::SearchWizard* ui;
    QList<QSqlRecord> _selectedRecords;

    PatternButton * _patternHelperButton;
    QThread * _thread;
    WidgetAppender* _widgetAppender;
};

#endif // SEARCHWIZARD_H
