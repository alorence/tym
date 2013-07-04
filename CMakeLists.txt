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

# Automatically add to each subdir the current source and binary dirs to include path
set(CMAKE_INCLUDE_CURRENT_DIR ON)
project(tym CXX)
cmake_minimum_required(VERSION 2.8.9)

############# Build Settings ##############
option(TYM_UPDATE_TRANSLATIONS OFF)

# Setup the version numbers
set(TYM_MAJOR_VERSION 0)
set(TYM_MINOR_VERSION 2)
set(TYM_PATCH_VERSION 1)

set(TYM_BETA 1)
################## END ####################

# Include some macros usable accross cmake projects
include(resources/config/TymUsefulMacros.cmake)

# Configure projects to use Qt5
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type: Default (No optimizations, no debug symbols)")
else()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

if(Qt5Core_FOUND)
    message(STATUS "Qt found, version ${Qt5Core_VERSION_STRING}")
endif()

if(MSVC) # MSVC
    # Display more warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3")

elseif(UNIX OR CMAKE_COMPILER_IS_GNUCXX)
    # Enable C++11 and displays all warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

    if(APPLE)
        # Required on OSX to compile c++11
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -mmacosx-version-min=10.7")
    endif(APPLE)

endif()

today(TYM_BUILD_DATE)
set(TYM_SHORT_VERSION "${TYM_MAJOR_VERSION}.${TYM_MINOR_VERSION}.${TYM_PATCH_VERSION}")
set(TYM_VERSION "${TYM_MAJOR_VERSION}.${TYM_MINOR_VERSION}.${TYM_PATCH_VERSION}.${TYM_BUILD_DATE}")

set(TYM_NAME "Tag Your Music")
if(APPLE)
    set(TYM_EXECUTABLE_FILENAME "Tag Your Music")
else()
    set(TYM_EXECUTABLE_FILENAME tym)
endif()
set(TYM_BIN_DIR "${CMAKE_SOURCE_DIR}/bin")
# Path where all dlls will be copied before compiling package
set(TYM_SETUP_TREE_PATH ${CMAKE_CURRENT_BINARY_DIR}/package-skeleton)
# Directory were resulting package will be generated
set(TYM_SETUP_OUTPUT_PATH ${TYM_BIN_DIR})
# Resulting package filename
if(TYM_BETA)
    set(TYM_SETUP_FILENAME TagYourMusic-${TYM_SHORT_VERSION}-beta)
else()
    set(TYM_SETUP_FILENAME TagYourMusic-${TYM_SHORT_VERSION})
endif()

include_directories(libs/cutelogger/include)
add_subdirectory(libs/cutelogger)

# Parse subdirectories
add_subdirectory(resources)
add_subdirectory(src)
add_subdirectory(packages)

add_custom_target(docs
    COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
)
