/**
* Copyright 2012 Antoine Lorence. All right reserved.
*
* This file is part of TYM (Tag your Library).
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

#include "patterntool.h"

PatternTool::PatternTool(QString pattern, QObject *parent) :
    QObject(parent),
    pattern(pattern)
{
    QStringList list = pattern.split('%', QString::SkipEmptyParts);
    QStringList regExpList;

    for(int i = 0 ; i < list.count() ; ++i) {

        QString elt = list.at(i);

        QString classicRexpPattern = "(.+)";

        if(elt == "ID" || elt == "id") {
            regExpList << "([0-9]+)";
            replacementMap[i+1] = "bpid";
        } else if(elt == "ARTISTS" || elt == "artists") {
            regExpList << classicRexpPattern;
            replacementMap[i+1] = "artists";
        } else if(elt == "TITLE" || elt == "title") {
            regExpList << classicRexpPattern;
            replacementMap[i+1] = "title";
        } else if(elt == "EXT" || elt == "ext") {
            regExpList << "([a-zA-Z0-9]{2,4})";
            replacementMap[i+1] = "ext";
        } else if(elt == "OTHER" || elt == "other") {
            regExpList << "(.+)";
        } else {
            regExpList << QString("(%1)").arg(elt.replace(".", "\\."));
        }
    }
    regExpList.prepend("^");
    regExpList.append("$");

    rexp = QRegExp(regExpList.join(""), Qt::CaseInsensitive, QRegExp::RegExp2);
}

QMap<QString, QString> PatternTool::parseValues(QString &source, const QStringList & interestingKeys) const
{
    QMap<QString, QString> result;
    if(rexp.indexIn(source) != -1) {
        foreach(int i, replacementMap.keys()) {
            if(interestingKeys.contains(replacementMap[i])) {
                result[replacementMap[i]] = rexp.cap(i);
            }
        }
    }
    return result;
}
