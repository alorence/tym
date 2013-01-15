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

#ifndef SEARCHTHREAD_H
#define SEARCHTHREAD_H

#include <QThread>

#include "tools/patterntool.h"
#include "network/searchprovider.h"
#include "wizards/searchwizard.h"

class SearchThread : public QThread
{
    Q_OBJECT
public:
    explicit SearchThread(QString, SearchWizard::SearchType, QList<QSqlRecord> selectedRecords, QObject *parent = 0);
    void run();
    
signals:
    
public slots:

private:
    QString _searchPattern;
    SearchWizard::SearchType _searchType;
    QList<QSqlRecord> _selectedRecords;
};

#endif // SEARCHTHREAD_H
