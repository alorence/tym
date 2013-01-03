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

    # TODO : Copy debug version of dlls in Debug BUILD_TYPE
    add_custom_target(makesetup

        COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/QtCore4.dll ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/QtGui4.dll ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/QtSql4.dll ${TYM_SETUP_TREE_PATH}
        COMMAND ${CMAKE_COMMAND} -E copy ${QT_BINARY_DIR}/QtNetwork4.dll ${TYM_SETUP_TREE_PATH}

        COMMAND iscc ${CMAKE_CURRENT_BINARY_DIR}/tym-setup.iss

        COMMENT "Building installer with Inno Setup compiler (iscc)."
        SOURCES win/tym-setup.iss.in
    )
else()
    # TBD
endif()