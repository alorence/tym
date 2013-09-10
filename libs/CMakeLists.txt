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

find_package(Qt5Script REQUIRED)
find_package(Qt5Network REQUIRED)

set(TYM_LIBS_INCLUDES
	${CMAKE_CURRENT_SOURCE_DIR}/cutelogger/include
	${CMAKE_CURRENT_SOURCE_DIR}/o2/src
	PARENT_SCOPE
)

# CuteLogger (logging library)
add_subdirectory(cutelogger)

# o2 (OAuth authentication for Qt)
set(o2_HDRS
	o2/src/o1.h
    o2/src/o1requestor.h
	o2/src/o2abstractstore.h
	o2/src/o2globals.h
    o2/src/o2replyserver.h
    o2/src/o2settingsstore.h
	o2/src/simplecrypt.h
)
set(o2_SRC
	o2/src/o1.cpp
    o2/src/o1requestor.cpp
    o2/src/o2replyserver.cpp
    o2/src/o2settingsstore.cpp
	o2/src/simplecrypt.cpp
)

add_library(o2 STATIC ${o2_SRC} ${o2_HDRS})
qt5_use_modules(o2 Network)
