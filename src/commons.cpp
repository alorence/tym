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

#include "commons.h"

const QString Constants::dynamicPictureUrl()
{
    return "http://geo-media.beatport.com/image_size/200x200/%1.jpg";
}

const QString Constants::dataLocation()
{
    // FIXME : On windows XP, AppData path contains spaces. replace() at the end of line must be removed
    return QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation).replace(' ', "");
}

const QString Constants::picturesLocation()
{
    return dataLocation() + QDir::separator() + "albumarts";
}
