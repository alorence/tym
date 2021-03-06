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
    task.h
    taskmonitor.h
    export/exporttask.h
    export/exportconfigurator.h
    libupdater/librarystatusupdater.h
    rename/renametask.h
    rename/renameconfigurator.h
    search/searchtask.h
    search/searchconfigurator.h
)

set(CPP
    task.cpp
    taskmonitor.cpp
    export/exporttask.cpp
    export/exportconfigurator.cpp
    libupdater/librarystatusupdater.cpp
    rename/renametask.cpp
    rename/renameconfigurator.cpp
    search/searchtask.cpp
    search/searchconfigurator.cpp
)

set(UIS
    taskmonitor.ui
    rename/renameconfigurator.ui
    export/exportconfigurator.ui
    search/searchconfigurator.ui
)

qt5_wrap_ui(UIS_HDRS ${UIS})

add_library(TymTasks STATIC ${CPP} ${HDRS} ${UIS_HDRS})
qt5_use_modules(TymTasks Widgets Sql Network)
