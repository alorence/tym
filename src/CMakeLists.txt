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

# Parse subdirectories
add_subdirectory(dbaccess)
add_subdirectory(network)
add_subdirectory(wizards)
add_subdirectory(gui)

set(tym_HEADERS
    # commons.h is not a QObject and MOC doesn't need to be applied on it
    patterntool.h
)

set(tym_SOURCES
    commons.cpp
    main.cpp
    patterntool.cpp
)

# Process headers files (<filename>.h) with moc, generate corresponding moc_<filename>.cxx
# These process converts specific Qt stuff (signals/slots, etc.) into valid c++ equivalent
qt5_wrap_cpp(tym_MOCS ${tym_HEADERS})

# Compile translations (*.ts) files
qt5_add_translation(TYM_TRANSLATIONS ${TRANSLATIONS_FILES})

# build/gui/ui_mainwindow.h must be accessible for main.cpp
include_directories(${tymgui_BINARY_DIR})

if(${WIN32})
    set(GUI_FLAG WIN32)
elseif(${APPLE})
    set(GUI_FLAG MACOSX_BUNDLE)
endif(${WIN32})

# Configure final executable building
add_executable(${TYM_EXECUTABLE_FILENAME}
    ${GUI_FLAG}
    ${tym_SOURCES} ${tym_MOCS}
    $<TARGET_OBJECTS:dbaccess>
    $<TARGET_OBJECTS:network>
    $<TARGET_OBJECTS:gui>
    $<TARGET_OBJECTS:wizards>
    $<TARGET_OBJECTS:resources>
    ${TYM_TRANSLATIONS}
)

# Add Qt5 modules dependencies
qt5_use_modules(${TYM_EXECUTABLE_FILENAME}
    Gui
    Widgets
    Sql
    Network
    LinguistTools
)

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
