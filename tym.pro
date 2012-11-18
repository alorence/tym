#-------------------------------------------------
#
# Project created by QtCreator 2012-08-29T10:38:04
#
#-------------------------------------------------
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

QT       += core gui sql network
CONFIG   += debug_and_release

TARGET = tym
TEMPLATE = app

include(src/src.pri)
include(uis/uis.pri)

SOURCES += libs/qt-json/json.cpp

HEADERS  += libs/qt-json/json.h

RESOURCES += resources.qrc

TRANSLATIONS =  translations/tym_en.ts \
                translations/tym_fr.ts


