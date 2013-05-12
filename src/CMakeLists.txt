#
# Copyright 2012 Antoine Lorence. All right reserved.
#
# This file is part of TYM (Tag Your Music).
#
# TYM (Tag Your Music) is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TYM (Tag Your Music) is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TYM (Tag Your Music).  If not, see <http://www.gnu.org/licenses/>.
#
# Help on using CMake for Qt projects : https://qt-project.org/quarterly/view/using_cmake_to_build_qt_projects

# For subdirectories, add src & libs folders to include path
include_directories(.)

configure_file(version.h.in version.h)

set(tym_HEADERS
    concretetasks/task.h
    concretetasks/exportplaylisttask.h
    concretetasks/renametask.h
    concretetasks/searchtask.h
    concretetasks/librarystatusupdater.h
    tools/patterntool.h
    tools/searchprovider.h
    tools/picturedownloader.h
    tools/widgetchangesobserver.h
    dbaccess/bpdatabase.h
    dbaccess/librarymodel.h
    dbaccess/searchresultsmodel.h
    wizards/searchwizard.h
    wizards/renamewizard.h
    wizards/exportplaylistwizard.h
    gui/about.h
    gui/mainwindow.h
    gui/settingsdialog.h
    gui/trackinfosview.h
    gui/consolewidget.h
    gui/patternbutton.h
)

# These headers doesn't define QObject children,
# they does not need to be parsed with moc tool.
# They are added to the source list to appear in
# IDE project (QtCreator, Xcode, etc.)
set(tym_BASIC_HDRS
    commons.h
    tools/utils.h
    tools/patternelement.h
    dbaccess/libraryentry.h
)

set(tym_SOURCES
    main.cpp
    concretetasks/task.cpp
    concretetasks/exportplaylisttask.cpp
    concretetasks/renametask.cpp
    concretetasks/searchtask.cpp
    concretetasks/librarystatusupdater.cpp
    tools/patterntool.cpp
    tools/patternelement.cpp
    tools/searchprovider.cpp
    tools/picturedownloader.cpp
    tools/widgetchangesobserver.cpp
    tools/utils.cpp
    dbaccess/bpdatabase.cpp
    dbaccess/librarymodel.cpp
    dbaccess/libraryentry.cpp
    dbaccess/searchresultsmodel.cpp
    wizards/searchwizard.cpp
    wizards/renamewizard.cpp
    wizards/exportplaylistwizard.cpp
    gui/about.cpp
    gui/mainwindow.cpp
    gui/settingsdialog.cpp
    gui/trackinfosview.cpp
    gui/patternbutton.cpp
)

set(tym_UIS
    wizards/searchwizard.ui
    wizards/renamewizard.ui
    wizards/exportplaylistwizard.ui
    gui/mainwindow.ui
    gui/settingsdialog.ui
    gui/trackinfosview.ui
    gui/consolewidget.ui
    gui/about.ui
)

# Process headers files (<filename>.h) with moc, generate corresponding moc_<filename>.cxx
# These process converts specific Qt stuff (signals/slots, etc.) into valid c++ equivalent
qt5_wrap_cpp(tym_MOCS ${tym_HEADERS})

# Process forms files (<filename>.ui) and converts them to ui_<filename>.h files
qt5_wrap_ui(tym_UIS_HEADERS ${tym_UIS})

if(${WIN32})
    set(GUI_FLAG WIN32)
elseif(${APPLE})
    set(GUI_FLAG MACOSX_BUNDLE)
endif(${WIN32})

# Configure final executable building
add_executable(${TYM_EXECUTABLE_FILENAME}
    ${GUI_FLAG}
    ${tym_SOURCES}
    ${tym_BASIC_HDRS}
    ${tym_MOCS}
    ${tym_UIS_HEADERS}
    ${tym_resources}
)

target_link_libraries(${TYM_EXECUTABLE_FILENAME} Logger)

# Add Qt5 modules dependencies
qt5_use_modules(${TYM_EXECUTABLE_FILENAME}
    Core
    Widgets
    Sql
    Network
)

if(MSVC)
    # Link with Qt WinMain, ensuring that the entry function will have
    # signature required by MSVC
    target_link_libraries(${TYM_EXECUTABLE_FILENAME} Qt5::WinMain)
endif(MSVC)
