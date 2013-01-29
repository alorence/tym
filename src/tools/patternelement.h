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

#ifndef PATTERNELEMENT_H
#define PATTERNELEMENT_H

#include "commons.h"

class PatternElement
{
public:
    PatternElement();
    PatternElement(QString displayName, QString description, QString inRegExp, TrackFullInfos::Indexes sqlIndex = TrackFullInfos::InvalidIndex);

    const QString &displayName() const { return _displayName; }
    const QString &description() const { return _description; }
    const QString &inRegExp() const { return _regExp; }
    TrackFullInfos::Indexes sqlIndex() const { return _sqlIndex; }

    void setDisplayName(const QString& displayName) { _displayName = displayName; }
    void setDescription(const QString& description) { _description = description; }
    void setRegExp(const QString& inRegExp) { _regExp = inRegExp; }
    void setSqlIndex(TrackFullInfos::Indexes index) { _sqlIndex = index; }

private:
    QString _displayName;
    QString _description;
    QString _regExp;
    TrackFullInfos::Indexes _sqlIndex;
};

#endif // PATTERNELEMENT_H
