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

if(WIN32)
    set(TYM_SETUP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/win)
    configure_file(win/tym-setup.iss.in tym-setup.iss)

    if(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        set(CURRENT_CONFIGURATION DEBUG)
    else(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
        set(CURRENT_CONFIGURATION RELEASE)
    endif(CMAKE_BUILD_TYPE MATCHES Debug OR CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)

    get_target_property(CoreDll         Qt5::Core           IMPORTED_LOCATION_${CURRENT_CONFIGURATION})
    get_target_property(WidgetsDll      Qt5::Widgets        IMPORTED_LOCATION_${CURRENT_CONFIGURATION})
    get_target_property(GuiDll          Qt5::Gui            IMPORTED_LOCATION_${CURRENT_CONFIGURATION})
    get_target_property(NetworkDll      Qt5::Network        IMPORTED_LOCATION_${CURRENT_CONFIGURATION})
    get_target_property(SqlDll          Qt5::Sql            IMPORTED_LOCATION_${CURRENT_CONFIGURATION})

    find_file(qSqllitePlugin qsqlite.dll PATHS "${Qt5Core_DIR}/../../../plugins/sqldrivers")
    find_file(qJpegPlugin qjpeg.dll PATHS "${Qt5Core_DIR}/../../../plugins/imageformats")
    find_file(qWindowsPlugin qwindows.dll PATHS "${Qt5Core_DIR}/../../../plugins/platforms")
    find_file(icudtPlugin icudt49.dll PATHS "${Qt5Core_DIR}/../../../bin")
    find_file(icuinPlugin icuin49.dll PATHS "${Qt5Core_DIR}/../../../bin")
    find_file(icuucPlugin icuuc49.dll PATHS "${Qt5Core_DIR}/../../../bin")
    find_file(libGLES libGLESv2.dll PATHS "${Qt5Core_DIR}/../../../bin")
    find_file(libEGL libEGL.dll PATHS "${Qt5Core_DIR}/../../../bin")
    find_file(d3dDll D3DCompiler_43.dll PATHS "${Qt5Core_DIR}/../../../bin")

    # TODO : Copy debug version of dlls in Debug BUILD_TYPE
    add_custom_target(setup
        COMMAND ${CMAKE_COMMAND} -E copy ${CoreDll}     ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${WidgetsDll}  ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${GuiDll}      ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${NetworkDll}  ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${SqlDll}      ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${icudtPlugin} ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${icuinPlugin} ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${icuucPlugin} ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${libGLES}     ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${libEGL}      ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${d3dDll}      ${TYM_SETUP_TREE_PATH}

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

    add_dependencies(setup ${TYM_EXECUTABLE_FILENAME} Logger)
else()
    # TBD
endif()
