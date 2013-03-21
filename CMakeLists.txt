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

project(tym CXX)
cmake_minimum_required(VERSION 2.8.9)

# Include some macros usable accross cmake projects
include(resources/config/TymUsefulMacros.cmake)

# Configure projects to use Qt5
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

# Automatically add to each subdir the current source and binary dirs to include path
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(CMAKE_COMPILER_IS_GNUCXX)
    # Display all warnings
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

    # qt5/qtbase/include/QtCore/qglobal.h:1765:4: error:
    # #error "You must build your code with position independent code if Qt was
    # built with -reduce-relocations. " "Compile your code with -fPIC or -fPIE."
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    # Enable C++11
    SET(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")

endif(CMAKE_COMPILER_IS_GNUCXX)

# Setup the version numbers
set(TYM_MAJOR_VERSION 0)
set(TYM_MINOR_VERSION 1)
set(TYM_PATCH_VERSION 1)
set(TYM_BUILD_VERSION 0)

set(TYM_BETA 1)

set(TYM_SHORT_VERSION "${TYM_MAJOR_VERSION}.${TYM_MINOR_VERSION}.${TYM_PATCH_VERSION}")
set(TYM_VERSION "${TYM_MAJOR_VERSION}.${TYM_MINOR_VERSION}.${TYM_PATCH_VERSION}.${TYM_BUILD_VERSION}")

# Fix outpout paths and names
set(TYM_EXECUTABLE_FILENAME tym)
set(TYM_BIN_DIR "${CMAKE_SOURCE_DIR}/bin")
if("" STREQUAL "${CMAKE_BUILD_TYPE}")
    set(TYM_OUTPUT_PATH "${TYM_BIN_DIR}/Default")
else()
    set(TYM_OUTPUT_PATH "${TYM_BIN_DIR}/${CMAKE_BUILD_TYPE}")
endif()

set(EXECUTABLE_OUTPUT_PATH ${TYM_OUTPUT_PATH})
set(LIBRARY_OUTPUT_PATH ${TYM_OUTPUT_PATH})

# Path where all dlls will be copied before compiling package
set(TYM_SETUP_TREE_PATH ${TYM_OUTPUT_PATH})
# Directory were resulting package will be generated
set(TYM_SETUP_OUTPUT_PATH ${TYM_BIN_DIR})
# Resulting package filename
if(TYM_BETA)
    set(TYM_SETUP_FILENAME TagYourMusic-${TYM_SHORT_VERSION}-beta)
else(TYM_BETA)
    set(TYM_SETUP_FILENAME TagYourMusic-${TYM_SHORT_VERSION})
endif(TYM_BETA)

# Set the list of translations (*.ts) files to compile them in src directory
file(GLOB TRANSLATIONS_FILES translations/*.ts)
# TODO: Add a custom target to create / update ts files in transl. dir.

include_directories(libs/cutelogger/include)
add_subdirectory(libs/cutelogger)

# Parse subdirectories
add_subdirectory(resources)
add_subdirectory(src)
add_subdirectory(packages)

add_custom_target(docs
    COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
)
