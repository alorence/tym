/*****************************************************************************
Copyright 2012-2013 Antoine Lorence. All right reserved.

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

#ifndef PATTERNELEMENT_H
#define PATTERNELEMENT_H

#include "commons.h"

//NOTE: Missing doc on some methods

/*!
 * \brief Encapsulate all informations about a pattern element.
 * Used by \link PatternTool class, a pattern (formatted as %PATTERN%) is used to parse
 * or build filename. It needs to have some informations attached, as a corresponding
 * regular expression, human readable informations or sql index.
 * \sa PatternTool
 * \sa FileBasenameParser
 * \sa FileBasenameFormatter
 */
class PatternElement
{
public:
    /*!
     * \brief Build an empty pattern elemnt
     */
    PatternElement();
    /*!
     * \brief Build a complete pattern element
     * \param displayName Human readable name
     * \param description Human readable description
     * \param inRegExp RegularExpression associated with the pattern, to parse filenames
     * \param sqlIndex Corresponding index in TrackFullInfos table
     */
    PatternElement(QString displayName, QString description, QString inRegExp, TrackFullInfos::TableIndexes sqlIndex = TrackFullInfos::InvalidIndex);
    /*!
     * \link _displayName
     */
    const QString &displayName() const { return _displayName; }
    /*!
     * \link _description
     */
    const QString &description() const { return _description; }
    const QString &inRegExp() const { return _regExp; }
    TrackFullInfos::TableIndexes sqlIndex() const { return _sqlIndex; }

    /*!
     * \link _displayName
     */
    void setDisplayName(const QString& displayName) { _displayName = displayName; }
    /*!
     * \link _description
     */
    void setDescription(const QString& description) { _description = description; }
    void setRegExp(const QString& inRegExp) { _regExp = inRegExp; }
    void setSqlIndex(TrackFullInfos::TableIndexes index) { _sqlIndex = index; }

private:
    /*!
     * \brief The human readable name of the pattern element
     */
    QString _displayName;
    /*!
     * \brief The human readable description of the pattern element
     */
    QString _description;
    QString _regExp;
    TrackFullInfos::TableIndexes _sqlIndex;
};

#endif // PATTERNELEMENT_H
