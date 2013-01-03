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

#ifndef PATTERNTOOL_H
#define PATTERNTOOL_H

#include <QtCore>
#include <QtSql>
#include "commons.h"

class PatternTool : public QObject
{
    Q_OBJECT
public:
    explicit PatternTool(QString pattern, QObject *parent = 0);

    QMap<QString, QString> parseValues(QString& source, const QStringList &interestingKeys) const;
    QString stringFromPattern(QSqlRecord &) const;
    
signals:
    
public slots:

private:
    QString pattern;

    QMap<int, QString> inReplacementMap;
    QRegExp inRegExp;
    
};

#endif // PATTERNTOOL_H
