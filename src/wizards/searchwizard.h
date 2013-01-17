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

#ifndef SEARCHWIZARD_H
#define SEARCHWIZARD_H

#include <QWizard>
#include <QtCore>
#include <QSqlRecord>

namespace Ui {
class SearchWizard;
}

class SearchWizard : public QWizard
{
    Q_OBJECT
    
public:
    enum SearchType {
        FromId,
        FromArtistTitle,
        Custom
    };

    explicit SearchWizard(QList<QSqlRecord> selectedRecords, QWidget *parent = 0);
    ~SearchWizard();
    void initializePage(int id);

    QString pattern() const;
    SearchType searchType() const;

public slots:
    void setPattern(QString value);

private slots:
    void idSearchSelected(bool);
    void titleArtistSearchSelected(bool);
    void customSearchSelected(bool);

    void searchThreadFinished();

private:
    Ui::SearchWizard *ui;
    SearchType type;
    QList<QSqlRecord> _selectedRecords;

    enum WizardPages {
        SelectPatternPage = 0,
        ResultPage
    };
};

#endif // SEARCHWIZARD_H
