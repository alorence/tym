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
    about.h
    mainwindow.h
    settingsdialog.h
    trackinfosview.h
    consolewidget.h
    patternbutton.h
)

set(CPP
    about.cpp
    mainwindow.cpp
    settingsdialog.cpp
    trackinfosview.cpp
    patternbutton.cpp
)

set(UIS
    mainwindow.ui
    settingsdialog.ui
    trackinfosview.ui
    consolewidget.ui
    about.ui
)

qt5_wrap_cpp(MOCS ${HDRS})
qt5_wrap_ui(UIS_HDRS ${UIS})

add_library(TymGui OBJECT ${CPP} ${MOCS} ${UIS_HDRS} ${HDRS})

tym_qt5_use_modules(TymGui
    Widgets
    Sql
    Network
)