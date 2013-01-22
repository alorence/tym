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

namespace Library {
    enum Indexes {
        Uid = 0,
        FilePath,
        Bpid,
        Status,
        Message
    };
    enum FileStatuses {
        New = 0x00,
        ResultsAvailable = 0x01,
        FileNotFound = 0x10
    };
    Q_DECLARE_FLAGS(FileStatus, FileStatuses)
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Library::FileStatus)
namespace BPTracks {
    enum Indexes {
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
        PictureId
    };
}
namespace TrackFullInfos {
    enum Indexes {
        Artists,
        Remixers,
        Genres,
        LabelName,
        Bpid,
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
        PictureId
    };
}
namespace SearchResults {
    enum Indexes {
        LibId = 0,
        Bpid,
        Track,
        DefaultFor
    };
}
class Constants {
public:
    static const QString dynamicPictureUrl();
    static const QString dataLocation();
    static const QString picturesLocation();
};
#endif // COMMONS_H
