# Tag Your Music: Changelog

## 0.1.0 First release
### Publish date : TBD
#### Features
 - Import audio files (*.wav, *.mp3, *.ogg, *.flac) into internal library
 - Search for associated content on Beatport with public api (http://api.beatport.com)
    - All kinds of informations can be retrieved (artsis, remixers, title, lable, genres, publish & release date, tempo, key, etc.)
 - Link each track with one search result
 - Rename tracks according to result linked
    - Some simple patterns are predefined
    - Simple replacement for platform dependent non-supported characters

#### Known bugs
 - Wizrds have a too large title size. Qt 5.0.0 bug, should be resolved in a future version (see https://bugreports.qt-project.org/browse/QTBUG-28099)
 - If a wizard is closed before task is finished, a warning appear in console output
 - QSettings does not retrieve values stored previously on Ubuntu

#### To implements before releasing
 - Produce *.nml playlist files (NI Traktor Pro) andd append all tracks informations inside
 - Implements a full settings dialog to save user preferences

## Features planned in future releases
 - Add a button widget to help user building a file pattern (for searching from informations or renaming a track)
 - Add more options when searching tracks informations
