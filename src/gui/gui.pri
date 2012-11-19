#
# Copyright 2012 Antoine Lorence. All right reserved.
#
# This file is part of TYM (Tag Your Music).
#
# TYM (Tag your Library) is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TYM (Tag your Library) is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with TYM (Tag your Library).  If not, see <http://www.gnu.org/licenses/>.
#

FORMS += \
    src/gui/mainwindow.ui \
    src/gui/trackinfosview.ui \
    src/gui/settingsdialog.ui \
    src/gui/about.ui

HEADERS += \
    src/gui/settingsdialog.h \
    src/gui/trackinfosview.h \
    src/gui/mainwindow.h

SOURCES += \
    src/gui/settingsdialog.cpp \
    src/gui/trackinfosview.cpp \
    src/gui/mainwindow.cpp
