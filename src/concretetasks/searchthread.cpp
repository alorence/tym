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

#include "searchthread.h"

SearchThread::SearchThread(QString searchPattern, SearchWizard::SearchType searchType, QList<QSqlRecord> selectedRecords, QObject *parent) :
    QThread(parent),
    _searchPattern(searchPattern),
    _searchType(searchType),
    _selectedRecords(selectedRecords)
{
}

void SearchThread::run()
{
    PatternTool pt(_searchPattern);
    QMap<QString, QMap<QString, QString> > parsedValueMap;

    QStringList interestingKeys;
    if(_searchType == SearchWizard::FromId) {
        interestingKeys << "bpid";
    } else {
        interestingKeys << "artists" << "title" << "remixers" << "name" << "mixname" << "label";
    }

    foreach (QSqlRecord record, _selectedRecords) {
        QFileInfo file(record.value(Library::FilePath).toString());

        QString refFileName(file.fileName());
        parsedValueMap[record.value(Library::Uid).toString()] = pt.parseValues(refFileName, interestingKeys);
    }

//    ui->progress->setVisible(true);
//    ui->progress->setValue(ui->progress->minimum());

    // TODO : Get this value from settings
    SearchProvider searchProvider(QUrl("http://api.beatport.com"));

    QMap<QString, QString> * requestMap = new QMap<QString, QString>();
    if(_searchType == SearchWizard::FromId) {
        foreach(QString key, parsedValueMap.keys()) {
            QString bpid = parsedValueMap[key]["bpid"];
            if(bpid.isEmpty()) continue;

            requestMap->insert(key, bpid);
        }
        if( ! requestMap->isEmpty()) {
//            ui->progress->setMaximum(requestMap->size());
            searchProvider.searchFromIds(requestMap);
        }
    } else {
        foreach(QString key, parsedValueMap.keys()) {
            if(parsedValueMap[key].values().isEmpty()) continue;

            requestMap->insert(key, ((QStringList) parsedValueMap[key].values()).join(" "));
        }
        if( ! requestMap->isEmpty()) {
//            ui->progress->setMaximum(requestMap->size());
            searchProvider.searchFromName(requestMap);
        }
    }

    // Run the event loop,
    exec();
}
