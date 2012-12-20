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
include_directories(${tymlibs_SOURCE_DIR} .)

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

set(TRANSLATIONS
    ../${TRANSLATIONS_DIR}/tym_en.ts
    ../${TRANSLATIONS_DIR}/tym_fr.ts
)

qt4_add_translation(TRS ${TRANSLATIONS})

# Process headers files (<filename>.h) with moc, generate corresponding moc_<filename>.cxx
# These process converts specific Qt stuff (signals/slots, etc.) into valid c++ equivalent
qt4_wrap_cpp(tym_MOCS ${tym_HEADERS})

# build/gui/ui_mainwindow.h must be accessible for main.cpp
include_directories(${tymgui_BINARY_DIR})

# Configure final executable building
add_executable(tym
    ${tym_SOURCES} ${tym_MOCS}
    $<TARGET_OBJECTS:dbaccess>
    $<TARGET_OBJECTS:network>
    $<TARGET_OBJECTS:gui>
    $<TARGET_OBJECTS:wizards>
    $<TARGET_OBJECTS:resources>
    ${TRS}
)


# Set dependency for final executable
target_link_libraries(tym ${QT_LIBRARIES} json)
