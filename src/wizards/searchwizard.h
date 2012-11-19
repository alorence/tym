/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag Your Music).
*
* TYM (Tag your Library) is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* TYM (Tag your Library) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with TYM (Tag your Library).  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SEARCHWIZARD_H
#define SEARCHWIZARD_H

#include <QWizard>
#include <QtCore>

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

    explicit SearchWizard(QWidget *parent = 0);
    ~SearchWizard();
    QString pattern() const;
    SearchType searchType() const;

public slots:
    void setPattern(QString value);

private slots:
    void idSearchSelected(bool);
    void titleArtistSearchSelected(bool);
    void customSearchSelected(bool);

private:
    Ui::SearchWizard *ui;
    SearchType type;
};

#endif // SEARCHWIZARD_H
