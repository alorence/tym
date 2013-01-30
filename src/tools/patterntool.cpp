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

#include "patterntool.h"
#include <Logger.h>

PatternTool::PatternTool(const QString &pattern, QObject *parent) :
    QObject(parent)
{
    _commonRegExpPattern = ".+";

    _allowedPatterns["ID"] = PatternElement("Id", "Beatport's unique identifier", "[0-9]+", TrackFullInfos::Bpid);
    _allowedPatterns["NAME"] = PatternElement("Name", "Track's name", _commonRegExpPattern, TrackFullInfos::TrackName);
    _allowedPatterns["MIXNAME"] = PatternElement("Mixname", "Track's mixname", _commonRegExpPattern, TrackFullInfos::MixName);
    _allowedPatterns["TITLE"] = PatternElement("Title", "Track's title, usually correspond to %NAME% (%MIXNAME%)", _commonRegExpPattern, TrackFullInfos::Title);
    _allowedPatterns["ARTISTS"] = PatternElement("Artists", "Track's artists list", _commonRegExpPattern, TrackFullInfos::Artists);
    _allowedPatterns["REMIXERS"] = PatternElement("Remixers", "Track's remixers list", _commonRegExpPattern, TrackFullInfos::Remixers);
    _allowedPatterns["LABEL"] = PatternElement("Label", "Track's label", _commonRegExpPattern, TrackFullInfos::Label);

    setPattern(pattern);
}

void PatternTool::setPattern(const QString &pattern)
{
    _patternElts = pattern.split('%', QString::SkipEmptyParts);
}

FileBasenameParser::FileBasenameParser(const QString &pattern, QObject *parent) :
    PatternTool(pattern, parent)
{
    _allowedPatterns["OTHER"] = PatternElement("Other", "Uninteresting text", _commonRegExpPattern);

    QStringList inRegExpList;
    foreach(QString patternElt, _patternElts) {
        QString capitalPatternElt = patternElt.toUpper();
        if(availablesPatterns().contains(capitalPatternElt)) {
            inRegExpList << QString("(?<%1>%2)").arg(capitalPatternElt).arg(availablesPatterns().value(capitalPatternElt).inRegExp());
        } else {
            inRegExpList << QString("%1").arg(patternElt.replace(".", "\\."));
        }
    }

    inRegExpList.prepend("^");
    inRegExpList.append("$");

    _parserRegularExpression = QRegularExpression(inRegExpList.join(""), QRegularExpression::CaseInsensitiveOption);

}

const QMap<QString, PatternElement> &FileBasenameParser::availablesPatterns() const
{
    return _allowedPatterns;
}

QMap<TrackFullInfos::Indexes, QString> FileBasenameParser::parse(const QString &basename) const
{
    QRegularExpressionMatch matchResult = _parserRegularExpression.match(basename);

    QMap<TrackFullInfos::Indexes, QString> result;
    foreach(QString patternElt, _patternElts) {
        QString capitalPatternElt = patternElt.toUpper();
        if(availablesPatterns().contains(capitalPatternElt)
                && availablesPatterns()[capitalPatternElt].sqlIndex() != TrackFullInfos::InvalidIndex) {

            result[availablesPatterns().value(capitalPatternElt).sqlIndex()] = matchResult.captured(capitalPatternElt);
            LOG_DEBUG(QString("Add result %1 for key %2").arg(matchResult.captured(capitalPatternElt)).arg(capitalPatternElt));
        }
    }
    return result;
}

FileBasenameFormatter::FileBasenameFormatter(const QString &pattern, QObject *parent) :
    PatternTool(pattern, parent)
{
}

const QMap<QString, PatternElement> &FileBasenameFormatter::availablesPatterns() const
{
    return _allowedPatterns;
}

QString FileBasenameFormatter::format(const QSqlRecord &trackInfoRecord) const
{
    QString result;

    foreach(QString patternElt, _patternElts) {

        if(availablesPatterns().contains(patternElt.toUpper())) {
            result.append(trackInfoRecord.value(availablesPatterns().value(patternElt.toUpper()).sqlIndex()).toString());
        } else {
            result.append(patternElt);
        }

    }

    return result;
}
