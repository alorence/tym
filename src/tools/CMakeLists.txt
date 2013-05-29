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

set(HDRS
    patterntool.h
    searchprovider.h
    picturedownloader.h
    widgetchangesobserver.h
)

set(CPP
    patterntool.cpp
    patternelement.cpp
    searchprovider.cpp
    picturedownloader.cpp
    widgetchangesobserver.cpp
    utils.cpp
)

set(BASIC_HDRS
    utils.h
    patternelement.h
)

qt5_wrap_cpp(MOCS ${HDRS})

add_library(TymTools OBJECT ${CPP} ${MOCS} ${BASIC_HDRS} ${HDRS})
add_dependencies(TymTools TymGui)

tym_qt5_use_modules(TymTools
    Widgets
    Network
    Sql
)
