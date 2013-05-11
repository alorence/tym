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

set(TYM_SETUP_TARGET_NAME "setup")

if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    set(CURRENT_CONFIGURATION DEBUG)
else(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    set(CURRENT_CONFIGURATION RELEASE)
endif(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)

set(TYM_FULL_PATH "${EXECUTABLE_OUTPUT_PATH}/${TYM_EXECUTABLE_FILENAME}")

set(QT5_INSTALL_DIR "${Qt5Core_DIR}/../../..")

if(WIN32)
    set(TYM_SETUP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/win)
    configure_file(win/tym-setup.iss.in tym-setup.iss)

    get_target_property(Qt5CoreLib         Qt5::Core           IMPORTED_LOCATION_${CURRENT_CONFIGURATION})
    get_target_property(Qt5WidgetsLib      Qt5::Widgets        IMPORTED_LOCATION_${CURRENT_CONFIGURATION})
    get_target_property(Qt5GuiLib          Qt5::Gui            IMPORTED_LOCATION_${CURRENT_CONFIGURATION})
    get_target_property(Qt5NetworkLib      Qt5::Network        IMPORTED_LOCATION_${CURRENT_CONFIGURATION})
    get_target_property(Qt5SqlLib          Qt5::Sql            IMPORTED_LOCATION_${CURRENT_CONFIGURATION})

    find_file(qSqllitePlugin "qsqlite${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${Qt5Core_DIR}/../../../plugins/sqldrivers")
    find_file(qJpegPlugin "qjpeg${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${Qt5Core_DIR}/../../../plugins/imageformats")
    find_file(qSqllitePlugin "${CMAKE_SHARED_LIBRARY_PREFIX}qsqlite${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${QT5_INSTALL_DIR}/plugins/sqldrivers")
    find_file(qJpegPlugin "${CMAKE_SHARED_LIBRARY_PREFIX}qjpeg${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${QT5_INSTALL_DIR}/plugins/imageformats")
    find_file(qWindowsPlugin "qwindows${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${QT5_INSTALL_DIR}/plugins/platforms")
    find_file(icudtPlugin "icudt49${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${QT5_INSTALL_DIR}/bin")
    find_file(icuinPlugin "icuin49${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${QT5_INSTALL_DIR}/bin")
    find_file(icuucPlugin "icuuc49${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${QT5_INSTALL_DIR}/bin")
    find_file(libGLES "libGLESv2${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${QT5_INSTALL_DIR}/bin")
    find_file(libEGL "libEGL${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${QT5_INSTALL_DIR}/bin")
    find_file(Qt5d3dLib "D3DCompiler_43${CMAKE_SHARED_LIBRARY_SUFFIX}" PATHS "${QT5_INSTALL_DIR}/bin")

    # TODO : Copy debug version of dynamic libs in Debug BUILD_TYPE
    add_custom_target(${TYM_SETUP_TARGET_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy ${Qt5CoreLib}          ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${Qt5WidgetsLib}       ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${Qt5GuiLib}           ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${Qt5NetworkLib}       ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${Qt5SqlLib}           ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${icudtPlugin}         ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${icuinPlugin}         ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${icuucPlugin}         ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${libGLES}             ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${libEGL}              ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${Qt5d3dLib}           ${TYM_SETUP_TREE_PATH}

        COMMAND ${CMAKE_COMMAND} -E make_directory              ${TYM_SETUP_TREE_PATH}/platforms
        COMMAND ${CMAKE_COMMAND} -E copy ${qWindowsPlugin}      ${TYM_SETUP_TREE_PATH}/platforms
        COMMAND ${CMAKE_COMMAND} -E make_directory              ${TYM_SETUP_TREE_PATH}/sqldrivers
        COMMAND ${CMAKE_COMMAND} -E copy ${qSqllitePlugin}      ${TYM_SETUP_TREE_PATH}/sqldrivers
        COMMAND ${CMAKE_COMMAND} -E make_directory              ${TYM_SETUP_TREE_PATH}/imageformats
        COMMAND ${CMAKE_COMMAND} -E copy ${qJpegPlugin}         ${TYM_SETUP_TREE_PATH}/imageformats

        COMMAND iscc ${CMAKE_CURRENT_BINARY_DIR}/tym-setup.iss

        COMMENT "Building installer with Inno Setup compiler (iscc)."
        SOURCES win/tym-setup.iss.in
    )

    add_dependencies(${TYM_SETUP_TARGET_NAME} ${TYM_EXECUTABLE_FILENAME} Logger)
elseif(APPLE)
    
    add_custom_target(${TYM_SETUP_TARGET_NAME}

        COMMAND mkdir -p ${TYM_EXECUTABLE_FILENAME}.app/Contents/Frameworks
        COMMAND cp -r ${tym_BINARY_DIR}/src/${TYM_EXECUTABLE_FILENAME}.app/* ${TYM_EXECUTABLE_FILENAME}.app

        COMMAND ${QT5_INSTALL_DIR}/bin/macdeployqt ${TYM_EXECUTABLE_FILENAME}.app

        COMMAND rm -fr ${TYM_SETUP_OUTPUT_PATH}/${TYM_SETUP_FILENAME}.app
        COMMAND mv -f -v ${TYM_EXECUTABLE_FILENAME}.app ${TYM_SETUP_OUTPUT_PATH}/${TYM_SETUP_FILENAME}.app

        COMMAND rm -f ${TYM_SETUP_FILENAME}.zip
        COMMAND zip -v -r -X ./${TYM_SETUP_FILENAME}.zip ${TYM_SETUP_FILENAME}.app
        WORKING_DIRECTORY ${TYM_SETUP_OUTPUT_PATH}
        COMMENT "Build the app package"
    )
elseif(UNIX)
    
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "My funky project")
    set(CPACK_PACKAGE_VENDOR "Me, myself, and I")
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
    add_custom_target(${TYM_SETUP_TARGET_NAME}
        COMMAND echo "youpi"
    )
endif()
