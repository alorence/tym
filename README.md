# Tag Your Music - General informations

## Descritpion
A simple tool to get meta-informations on music tracks bought from Internet, working on MacOS, Windows and GNU/Linux
This is an open source software, under GPL v3. PLease read the LICENCE.html file to get more informations.

## Status
This project is under development, and not available to download for now. You can fork it or clone sources and try to compile your own version.

## Compile
To compile, run the classic commands
```bash
git clone git://github.com/alorence/tym.git
cd tym
git submodule init
git submodule update
mkdir build-<debug|release>
cd build-<debug|release>
cmake .. -DCMAKE_BUILD_TYPE=<Debug|Release>
<make|nmake>
```
The resulting executable is written in tym/bin folder

On Windows, it is possible to build the setup with the special target
```bash
nmake makesetup
```

## Dependencies
 - Modified version (https://gitorious.org/~alorence/cutelogger/alorences-cutelogger) of Cutelogger library (https://gitorious.org/cutelogger/cutelogger)
 - Qt 5.0.0 (https://qt-project.org/)
 - CMake 2.8.9 minimum
 - C++ compiler, g++ and msvc 2010 are ok, and others should also be
 - Inno Setup (Windows only)

## Bugs
Some bugs are known, please read the CHANGELOG.md. If you find a new one, please leave an issue on [issue tracker](https://github.com/alorence/tym/issues "Tag Your Music issues").


## Features
### In short terms
See CHANGES.md to get more informations

### In long terms
 - Write tages into FLAC / MP3 / OGG [/ WAV] files
 - Write special Traktor tag into music files to store non classic informations (BPM, Label, etc.)
 - Write specials Serato tags into music files
 - Search for tracks informations on another platforms (TBD)
If you have some needs or suggestions, please post on [issue tracker](https://github.com/alorence/tym/issues "Tag Your Music issues") with the "enhancement" tag
