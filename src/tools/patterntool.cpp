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

#include "patterntool.h"

#include "commons.h"

PatternTool::PatternTool(QString pattern, QObject *parent) :
    QObject(parent),
    pattern(pattern)
{
    QStringList list = pattern.split('%', QString::SkipEmptyParts);
    QStringList inRegExpList;

    for(int i = 0 ; i < list.count() ; ++i) {

        QString elt = list.at(i);

        QString classicRexpPattern = "(.+)";

        if(elt.toUpper() == "ID") {
            inRegExpList << "([0-9]+)";
            inReplacementMap[i+1] = "bpid";
        } else if(elt.toUpper() == "ARTISTS") {
            inRegExpList << classicRexpPattern;
            inReplacementMap[i+1] = "artists";
        } else if(elt.toUpper() == "TITLE") {
            inRegExpList << classicRexpPattern;
            inReplacementMap[i+1] = "title";
        } else if(elt.toUpper() == "EXT") {
            inRegExpList << "([a-zA-Z0-9]{2,4})";
            inReplacementMap[i+1] = "ext";
        } else if(elt.toUpper() == "OTHER") {
            inRegExpList << "(.+)";
        } else {
            inRegExpList << QString("(%1)").arg(elt.replace(".", "\\."));
        }
    }
    inRegExpList.prepend("^");
    inRegExpList.append("$");

    inRegExp = QRegExp(inRegExpList.join(""), Qt::CaseInsensitive, QRegExp::RegExp2);
}

QMap<QString, QString> PatternTool::parseValues(QString &source, const QStringList & interestingKeys) const
{
    QMap<QString, QString> result;
    if(inRegExp.indexIn(source) != -1) {
        foreach(int i, inReplacementMap.keys()) {
            if(interestingKeys.contains(inReplacementMap[i])) {
                result[inReplacementMap[i]] = inRegExp.cap(i);
            }
        }
    }
    return result;
}

QString PatternTool::stringFromPattern(QSqlRecord &trackInfoRecord) const
{
    QStringList partList = pattern.split('%', QString::SkipEmptyParts);

    QString result;

    foreach(QString part, partList) {
        if(part.toUpper() == "ARTISTS") {
            result.append(trackInfoRecord.value(TrackFullInfos::Artists).toString());
        } else if(part.toUpper() == "TITLE") {
            result.append(trackInfoRecord.value(TrackFullInfos::Title).toString());
        } else if(part.toUpper() == "NAME") {
            result.append(trackInfoRecord.value(TrackFullInfos::TrackName).toString());
        } else if(part.toUpper() == "MIXNAME") {
            result.append(trackInfoRecord.value(TrackFullInfos::MixName).toString());
        } else if(part.toUpper() == "LABEL") {
            result.append(trackInfoRecord.value(TrackFullInfos::LabelName).toString());
        } else {
            result.append(part);
        }
    }

    return result;
}
