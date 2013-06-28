# Tag Your Music: Changelog

### Known bugs
 - [Windows] Wizards have a too large title size. Qt 5.0.x bug, should be resolved in a future version (see https://bugreports.qt-project.org/browse/QTBUG-28099)

## 0.2.1 - Next release
#### New features

#### Minor improvements
 - Native shortucts have been set for common actions
 - Checkboxes are not displayed anymore. Classic selection is used to select files to be used for an action

#### Bug fixes
 - A missing DLL caused an error at startup, Windows version can now run normally
 - Fixed #2. When some files were missing at startup, entire library was not displayed immediatly
 - Actions are correctly disabled after a refresh, when selection is reset

## 0.2.0 - 2013-06-09
#### New features
 - Enable a manual search when only 1 track is selected in the library
 - Try to find and link the best result for classic searches
 - Tracks are displayed in a tree view instead of a flat one

#### Minor improvements
 - Display a progressbar while search run

#### Bug fixes
 - Compile errors on GNU/Linux
 - Files containing a '.' (dot) can now be searched automatically
 - Search can be performed on IDs on more than 10 tracks per request
 - Forbidden characters are replaced by a space when renaming (depending on OS specific rules)

## 0.1.0 First release - 2013-02-17
#### Features
 - Import audio files (*.wav, *.mp3, *.ogg, *.flac) into internal library
 - Search for associated content on Beatport with public api (http://api.beatport.com)
    - All kinds of informations can be retrieved (artists, remixers, title, label, genres, publish & release date, tempo, key, etc.)
 - Link each track with one search result
 - Rename tracks according to result linked
    - Some simple patterns are predefined
    - Simple replacement for platform dependent non-supported characters
 - Export informations as *.nml playlist(s) to display some informations in Traktor pro 2.6
