#
# Copyright 2012-2013 Antoine Lorence. All right reserved.
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

cmake_minimum_required(VERSION 2.8.9)
if(POLICY CMP0020)
   cmake_policy(SET CMP0020 OLD)
endif()

# Automatically add to each subdir the current source and binary dirs to include path
set(CMAKE_INCLUDE_CURRENT_DIR ON)
project(tym CXX)

############# Build Settings ##############
# Main name of the application
set(TYM_NAME "Tag Your Music")
# Main name of the application
set(TYM_DESCRIPTION "A graphical tool to manage DJ library's audio files.")

# Setup the version numbers
set(TYM_MAJOR_VERSION 0)
set(TYM_MINOR_VERSION 2)
set(TYM_PATCH_VERSION 2)

option(TYM_BETA OFF)
option(TYM_UPDATE_TRANSLATIONS OFF)
################## END ####################

# Include some macros usable accross cmake projects
include(resources/config/TymUsefulMacros.cmake)

# Detect current architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(TYM_ARCH "x64")
else()
    set(TYM_ARCH "x86")
endif()

# Configure build type (Release when no one is selected)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    set(BUILDTYPE_MSG "Release (set by default)")
else()
    set(BUILDTYPE_MSG "${CMAKE_BUILD_TYPE}")
endif()

# Configure projects to use Qt5
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5LinguistTools REQUIRED)

# Configure compile flags and various platform dependent variables
set(TYM_FINAL_TARGET "TymFinalBinary")
set(TYM_EXECUTABLE_FILENAME "tym")
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

# Get the current date
today(TYM_BUILD_DATE)
set(TYM_SHORT_VERSION "${TYM_MAJOR_VERSION}.${TYM_MINOR_VERSION}.${TYM_PATCH_VERSION}")
set(TYM_VERSION "${TYM_MAJOR_VERSION}.${TYM_MINOR_VERSION}.${TYM_PATCH_VERSION}.${TYM_BUILD_DATE}")
set(TYM_DELIVERABLES_OUTPUT "${CMAKE_SOURCE_DIR}/deliverables")

# Build dependencies
include_directories(libs/cutelogger/include)
add_subdirectory(libs/cutelogger)

# Parse subdirectories
add_subdirectory(resources)
add_subdirectory(src)
add_subdirectory(packages)
add_subdirectory(docs)

# Display summarry for configuration
message(STATUS "==============================================")
message(STATUS "Architecture:       ${TYM_ARCH}")
if(Qt5Core_FOUND)
    message(STATUS "Qt:                 Found (${Qt5Core_VERSION_STRING}) in directory ${Qt5Core_DIR}")
else()
    message(SEND_ERROR "Qt has not been found! Unable to build anything.")
endif()
message(STATUS "Build type:         ${BUILDTYPE_MSG}")
message(STATUS "Build flags:        ${CMAKE_CXX_FLAGS}")
message(STATUS "Version:            ${TYM_SHORT_VERSION}")
message(STATUS "Beta release:       ${TYM_BETA}")
message(STATUS "Update transl.:     ${TYM_UPDATE_TRANSLATIONS}")
message(STATUS "Target executable:  ${TYM_EXECUTABLE_FILENAME}")
message(STATUS "==============================================")

if(NOT APPLE)
    # Uninstall target
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/resources/config/TymUninstall.cmake.in"
        "${CMAKE_BINARY_DIR}/TymUninstall.cmake"
        IMMEDIATE @ONLY
    )
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/TymUninstall.cmake
    )
endif()
