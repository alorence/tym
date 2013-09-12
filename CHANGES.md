## Tag Your Music: Changelog

#### 0.3 - Next release
###### Bug fixes
###### Major improvements / new features
 - The GUI is now internationalized
 - Search, rename and export tasks have been rewritten
   - A new GUI helps to configure and control the results of a task execution
   - The core of each task have been cleaned and improved
   - Old wizards, which had a bad rendering on Windows, have been dropped
 - The new Beatport API, based on OAuth authentication have been implemented. To use the search feature, we now have to login with a valid Beatport account

###### Minor improvements
 - [Win] A separate setup package is built for x86 and x64 systems

#### 0.2.1 - 2013-07-11
###### Minor improvements
 - Native shortucts have been set for common actions
 - Checkboxes are not displayed anymore. Classic selection is used to specify files to use for the next action
 - Qt version has been updated to 5.1.0

###### Bug fixes
 - The MS Windows package works now. In the previous version, a missing DLL was preventing to run the application
 - [[#2](https://github.com/alorence/tym/issues/2)] When some audio files are missing at startup, library content is now displayed immediatly
 - Actions are correctly disabled after a refresh, when selection is reset

#### 0.2.0 - 2013-06-09
###### New features
 - Enable a manual search when only 1 track is selected in the library
 - After a search, the better result is automatically linked to the file
 - Tracks are displayed in a tree instead of a table view

###### Minor improvements
 - Display a progressbar while each action runs
 - Adding support for *.aiff files

###### Bug fixes
 - All compile errors have been fixed on GNU/Linux
 - Automatic search from files containing a '.' (dot) works normally
 - Search can be performed from IDs on more than 10 tracks per request
 - When renaming, forbidden characters (depending on OS specific rules) are replaced by a space

#### 0.1.0 First release - 2013-02-17
###### Features
 - Import audio files (*.wav, *.mp3, *.ogg, *.flac) into internal library
 - **Search** on Beatport public API (http://api.beatport.com) for information about tracks
    - All kinds of informations can be retrieved (artists, remixers, title, label, genres, picture, etc.)
    - Filenames are used to extract terms used to perform search
 - Link each track with a search result
 - **Rename** tracks according to result linked
    - Some simple patterns are predefined
    - Simple replacement for platform dependent non-supported characters
 - **Export** information as *.nml playlist(s) to display some informations in Native Instrument Traktor Pro 2
