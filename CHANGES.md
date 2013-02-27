# Tag Your Music: Changelog

## Known bugs
 - Wizards have a too large title size. Qt 5.0.0 bug, should be resolved in a future version (see https://bugreports.qt-project.org/browse/QTBUG-28099)

## Next release
### New features
 - Enable a manual search when only 1 library track is selected
 - Try to find and link the best result for classic searches
### Minor fixes
 - Compile error on GNU/Linux

## 0.1.0 First release
Publish date : 2013-02-17
#### Features
 - Import audio files (*.wav, *.mp3, *.ogg, *.flac) into internal library
 - Search for associated content on Beatport with public api (http://api.beatport.com)
    - All kinds of informations can be retrieved (artists, remixers, title, label, genres, publish & release date, tempo, key, etc.)
 - Link each track with one search result
 - Rename tracks according to result linked
    - Some simple patterns are predefined
    - Simple replacement for platform dependent non-supported characters
 - Export informations as *.nml playlist(s) to display some informations in Traktor pro 2.6

