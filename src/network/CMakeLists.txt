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

set(HDRS
    o1beatport.h
    searchprovider.h
    picturedownloader.h
)

set(CPP
    o1beatport.cpp
    searchprovider.cpp
    picturedownloader.cpp
)

set(UIS
    o1beatport.ui
)

qt5_wrap_ui(UIS_HDRS ${UIS})
add_library(TymNetwork STATIC ${CPP} ${HDRS} ${UIS_HDRS})
qt5_use_modules(TymNetwork Widgets Network Sql WebKitWidgets)
