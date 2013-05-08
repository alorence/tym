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

#ifndef COMMONS_H
#define COMMONS_H

#include <QtCore>

// General
#define TYM_PICTURE_URL_DOWNLOAD "http://geo-media.beatport.com/image_size/200x200/%1.jpg"
#define TYM_DATA_LOCATION QStandardPaths::writableLocation(QStandardPaths::DataLocation)
#define TYM_ALBUMARTS_LOCATION QStandardPaths::writableLocation(QStandardPaths::DataLocation)+"/albumarts"
#define TYM_SUPPORTED_SUFFIXES QStringList(QList<QString>() << "*.wav" << "*.flac" << "*.mp3" << "*.ogg")
#define TYM_BEATPORT_DEFAULT_FORMAT "%ID%_%OTHER%"

namespace Settings {
    enum KeyStyle {
        Beatport = 0,
        MixedInKey
    };
}

// Settings - interface
#define TYM_PATH_DISPLAY_FULLPATHS "settings/interface/library/fullpaths"
#define TYM_DEFAULT_DISPLAY_FULLPATHS false
#define TYM_PATH_DISPLAY_COLORS "settings/interface/library/colors"
#define TYM_DEFAULT_DISPLAY_COLORS true
#define TYM_PATH_DISPLAY_CHECKBOXES "settings/interface/library/checkboxes"
#define TYM_DEFAULT_DISPLAY_CHECKBOXES true
#define TYM_PATH_KEY_STYLE "settings/interface/keystyle"
#define TYM_DEFAULT_KEY_STYLE Settings::Beatport

// Settings - network
#define TYM_PATH_API_URL "settings/network/beatport/apihost"
#define TYM_DEFAULT_API_URL "http://api.beatport.com"
#define TYM_PATH_PROXY_ENABLED "settings/network/proxy/enabled"
#define TYM_DEFAULT_PROXY_ENABLED false
#define TYM_PATH_PROXY_HOST "settings/network/proxy/host"
#define TYM_DEFAULT_PROXY_HOST ""
#define TYM_PATH_PROXY_USER "settings/network/proxy/user"
#define TYM_DEFAULT_PROXY_USER ""
#define TYM_PATH_PROXY_PWD "settings/network/proxy/password"
#define TYM_DEFAULT_PROXY_PWD ""
#define TYM_PATH_PROXY_PORT "settings/network/proxy/port"
#define TYM_DEFAULT_PROXY_PORT ""

namespace Library {
    enum TableIndexes {
        Uid = 0,
        FilePath,
        Bpid,
        Status,
        NumResults
    };
    enum FileStatuses {
        New = 0x00,
        Searched = 0x01,
        FileNotFound = 0x10
    };
    Q_DECLARE_FLAGS(FileStatus, FileStatuses)
    enum GuiIndexes {
        Name = 0,
        StatusMessage,
        Results,
        Infos
    };
}
Q_DECLARE_OPERATORS_FOR_FLAGS(Library::FileStatus)
namespace BPTracks {
    enum TableIndexes {
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
    enum TableIndexes {
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
        PictureId,
        InvalidIndex = 256
    };
}
namespace SearchResults {
    enum TableIndexes {
        LibId = 0,
        Bpid,
        Track,
        DefaultFor
    };
}

#endif // COMMONS_H
