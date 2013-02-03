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
    # commons.h # Not a QObject and MOC doesn't need to be applied on it
    concretetasks/task.h
    concretetasks/renametask.h
    concretetasks/searchtask.h
    concretetasks/librarystatusupdater.h
    tools/patterntool.h
    # tools/patternelement.h # Not a QObject
    tools/searchprovider.h
    tools/picturedownloader.h
    tools/widgetchangesobserver.h
    dbaccess/bpdatabase.h
    dbaccess/librarymodel.h
    dbaccess/searchresultsmodel.h
    wizards/searchwizard.h
    wizards/renamewizard.h
    gui/about.h
    gui/mainwindow.h
    gui/settingsdialog.h
    gui/trackinfosview.h
    gui/consolewidget.h
    gui/patternbutton.h
)

set(tym_SOURCES
    main.cpp
    concretetasks/task.cpp
    concretetasks/renametask.cpp
    concretetasks/searchtask.cpp
    concretetasks/librarystatusupdater.cpp
    tools/patterntool.cpp
    tools/patternelement.cpp
    tools/searchprovider.cpp
    tools/picturedownloader.cpp
    tools/widgetchangesobserver.cpp
    dbaccess/bpdatabase.cpp
    dbaccess/librarymodel.cpp
    dbaccess/searchresultsmodel.cpp
    wizards/searchwizard.cpp
    wizards/renamewizard.cpp
    gui/about.cpp
    gui/mainwindow.cpp
    gui/settingsdialog.cpp
    gui/trackinfosview.cpp
    gui/patternbutton.cpp
)

set(tym_UIS
    wizards/searchwizard.ui
    wizards/renamewizard.ui
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

# Compile translations (*.ts) files
qt5_add_translation(TYM_TRANSLATIONS ${TRANSLATIONS_FILES})

# build/gui/ui_mainwindow.h must be accessible for main.cpp
include_directories(${CMAKE_CURRENT_BINARY_DIR}/gui)

if(${WIN32})
    set(GUI_FLAG WIN32)
elseif(${APPLE})
    set(GUI_FLAG MACOSX_BUNDLE)
endif(${WIN32})

# Configure final executable building
add_executable(${TYM_EXECUTABLE_FILENAME}
    ${GUI_FLAG}
    ${tym_SOURCES}
    ${tym_MOCS}
    ${tym_UIS_HEADERS}
    ${TYM_TRANSLATIONS}
    $<TARGET_OBJECTS:resources>
)

target_link_libraries(${TYM_EXECUTABLE_FILENAME} Logger)

# Add Qt5 modules dependencies
qt5_use_modules(${TYM_EXECUTABLE_FILENAME}
    Gui
    Widgets
    Sql
    Network
    LinguistTools
)

# WIN32 specific flags and compiler options
if(WIN32)
    # Ensure main() function is compatible with target platform
    target_link_libraries(${TYM_EXECUTABLE_FILENAME} ${Qt5Core_QTMAIN_LIBRARIES})

    if(${MINGW})
        set_target_properties(${EXECUTABLE_FILENAME} PROPERTIES LINK_FLAGS "-mwindows")
    elseif(${MSVC})
        set_target_properties(${EXECUTABLE_FILENAME} PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
        set_target_properties(${EXECUTABLE_FILENAME} PROPERTIES COMPILE_DEFINITIONS_DEBUG "_CONSOLE")
        set_target_properties(${EXECUTABLE_FILENAME} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
        set_target_properties(${EXECUTABLE_FILENAME} PROPERTIES COMPILE_DEFINITIONS_RELWITHDEBINFO "_CONSOLE")
        set_target_properties(${EXECUTABLE_FILENAME} PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
        set_target_properties(${EXECUTABLE_FILENAME} PROPERTIES MINSIZEREL "/SUBSYSTEM:WINDOWS")
    endif(${MINGW})
endif(WIN32)
