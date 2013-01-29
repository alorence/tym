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

#ifndef PATTERNTOOL_H
#define PATTERNTOOL_H

#include <QtCore>
#include <QtSql>

#include "commons.h"
#include "patternelement.h"
/*!
 * \brief The PatternTool class
 *
 *
 */
class PatternTool : public QObject
{
    Q_OBJECT
public:
    explicit PatternTool(const QString & pattern, QObject *parent = 0);
    virtual const QMap<QString, PatternElement> & availablesPatterns() const = 0;
    void setPattern(const QString & pattern);

protected:
    QStringList _patternElts;
    QMap<QString, PatternElement> _allowedPatterns;
    QString _commonRegExpPattern;
};

class FileBasenameParser : public PatternTool
{
public:
    FileBasenameParser(const QString &pattern = QString(), QObject *parent = 0);
    const QMap<QString, PatternElement> & availablesPatterns() const;
    QMap<TrackFullInfos::Indexes, QString> parse(const QString &basename) const;

private:
    QRegularExpression _parserRegularExpression;
};

class FileBasenameFormatter : public PatternTool
{
public:
    FileBasenameFormatter(const QString &pattern = QString(), QObject *parent = 0);
    const QMap<QString, PatternElement> & availablesPatterns() const;
    QString format(const QSqlRecord &) const;

};

#endif // PATTERNTOOL_H
