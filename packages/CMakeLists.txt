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

# Name of the generated target
set(TYM_DELIVERABLE_TARGET "deliverable")

# Build the package basename
set(TYM_DELIVERABLE_BASENAME TagYourMusic-${TYM_SHORT_VERSION}-${TYM_ARCH})
if(TYM_BETA)
    set(TYM_DELIVERABLE_BASENAME ${TYM_DELIVERABLE_BASENAME}-beta)
endif()
if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    # Used for windows deliverable, to include the right version of Qt Dlls in the setup
    set(DEBUG_SUFFIX "d")
    set(TYM_DELIVERABLE_BASENAME ${TYM_DELIVERABLE_BASENAME}-debug)
endif()

# Get the Qt base dir
set(QT5_INSTALL_DIR "${Qt5Core_DIR}/../../..")
# Path where all dlls will be copied before compiling package
set(TYM_DELIVERABLE_TREE "${CMAKE_CURRENT_BINARY_DIR}/package-skeleton")
# Get the path of build executable name
get_property(TYM_FULL_PATH TARGET ${TYM_EXECUTABLE_FILENAME} PROPERTY LOCATION)

if(WIN32)
    configure_file(win/tym-setup.iss.in tym-setup.iss)
    configure_file(win/make-setup.bat.in make-setup.bat)
    configure_file(win/GPL3licence.txt ${TYM_DELIVERABLE_TREE}/GPL3licence.txt COPYONLY)

    add_custom_target(${TYM_DELIVERABLE_TARGET}
        COMMAND cmd /C make-setup.bat
        COMMENT "Building installer with Inno Setup compiler (iscc)."
    )

    add_dependencies(${TYM_DELIVERABLE_TARGET} ${TYM_EXECUTABLE_FILENAME})
elseif(APPLE)

    configure_file(osx/make-bundle.sh.in make-bundle.sh @ONLY)

    add_custom_target(${TYM_DELIVERABLE_TARGET}
        COMMAND sh ${CMAKE_CURRENT_BINARY_DIR}/make-bundle.sh
        WORKING_DIRECTORY ${TYM_DELIVERABLES_OUTPUT}
        COMMENT "Building the app package and the final zip"
    )
    add_dependencies(${TYM_DELIVERABLE_TARGET} ${TYM_EXECUTABLE_FILENAME})
elseif(UNIX)

    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A simple tool to manage DJ audio library")
    set(CPACK_PACKAGE_VENDOR "Antoine Lorence")
    #set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
    #set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
    set(CPACK_PACKAGE_VERSION_MAJOR ${TYM_MAJOR_VERSION})
    set(CPACK_PACKAGE_VERSION_MINOR ${TYM_MINOR_VERSION})
    set(CPACK_PACKAGE_VERSION_PATCH ${TYM_PATCH_VERSION})
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "TagYourMusic ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

    set(CPACK_STRIP_FILES ${EXECUTABLE_OUTPUT_PATH}/${TYM_EXECUTABLE_FILENAME})
    set(CPACK_SOURCE_STRIP_FILES "")

    set(CPACK_PACKAGE_EXECUTABLES "tym" "TagYourMusic")
    include(CPack)
    add_custom_target(${TYM_DELIVERABLE_TARGET}
        COMMAND echo "TBD - Linux package creation is not supported for now"
    )
endif()
