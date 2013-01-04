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

project(tymgui)

set(gui_HEADERS
    mainwindow.h
    settingsdialog.h
    trackinfosview.h
)

set(gui_SOURCES
    mainwindow.cpp
    settingsdialog.cpp
    trackinfosview.cpp
)

set(gui_UIS
    about.ui
    mainwindow.ui
    settingsdialog.ui
    trackinfosview.ui
)

# Process forms files (<filename>.ui) and converts them to ui_<filename>.h files
qt5_wrap_ui(gui_UIS_HEADERS ${gui_UIS})
# Process headers files (<filename>.h) with moc, generate corresponding moc_<filename>.cxx
# These process converts specific Qt stuff (signals/slots, etc.) into valid c++ equivalent
qt5_wrap_cpp(gui_MOCS ${gui_HEADERS})

add_library(gui OBJECT EXCLUDE_FROM_ALL ${gui_SOURCES} ${gui_MOCS} ${gui_UIS_HEADERS})
