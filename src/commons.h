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

#ifndef COMMONS_H
#define COMMONS_H

#include <QtCore>
#include <QtGui>

namespace LibraryIndexes {
    enum Index {
        Uid = 0,
        FilePath,
        Bpid,
        Status,
        Message
    };
}
namespace BPTracksIndexes {
    enum Index {
        Bpid = 0,
        TrackName,
        MixName,
        Title, // Full title, formatted as '%TrackName (%MixName)'
        Label,
        Key,
        Bpm,
        ReleaseDate,
        PublishDate,
        Price,
        Length,
        Release,
        ImageUrl,
        ImagePath
    };
}
namespace SearchResultsIndexes {
    enum Index {
        LibId = 0,
        Bpid,
        Track,
        DefaultFor
    };
}
namespace TracksPicturesIndexes {
    enum Index {
        Bpid = 0,
        Url = 1,
        Path = 16
    };
}
namespace FileStatus {
    enum Status {
        New = 0,
        ResultsAvailable = 1,
        FileNotFound = 16
    };
}
class Constants {
public:
    static const QString dynamicPictureUrl();
    static const QString dataLocation();
    static const QString picturesLocation();
};
#endif // COMMONS_H
