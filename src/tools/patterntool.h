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
 * \brief The PatternTool class is the base class for pattern manipulation.
 */
class PatternTool : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Build a PatternTool from \a pattern argument
     * \param pattern
     * \param parent
     */
    explicit PatternTool(const QString & pattern, QObject *parent = 0);
    /*!
     * \brief Return available pattern for a specific usage
     * \return
     */
    const QMap<QString, PatternElement> & availablesPatterns() const;
    void setPattern(const QString & pattern);

protected:
    QStringList _patternElts;
    QMap<QString, PatternElement> _allowedPatterns;
    QString _commonRegExpPattern;
};
/*!
 * \brief Define a parser for reading file name and extract some informations from it.
 */
class FileBasenameParser : public PatternTool
{
public:
    /*!
     * \copydoc PatternTool::PatternTool
     * \param pattern
     * \param parent
     */
    FileBasenameParser(const QString &pattern = QString(), QObject *parent = 0);
    /*!
     * \brief Extract informations from file basename
     * \param basename File name (without suffix)
     * \return Results as map, indexed by TrackFullInfos::Indexes
     */
    QMap<TrackFullInfos::Indexes, QString> parse(const QString &basename) const;

private:
    QRegularExpression _parserRegularExpression;
};

/*!
 * \brief Define a generator which produce file basenames according to pattern.
 */
class FileBasenameFormatter : public PatternTool
{
public:
    /*!
     * \copydoc PatternTool::PatternTool
     * \param pattern
     * \param parent
     */
    FileBasenameFormatter(const QString &pattern = QString(), QObject *parent = 0);
    /*!
     * \brief Format a file basename from \a pattern and \a infos in argument
     * \param infos QSqlRecord containg all informations about a track
     * \return A formatted basename (without suffix)
     */
    QString format(const QSqlRecord &infos) const;

};

#endif // PATTERNTOOL_H
