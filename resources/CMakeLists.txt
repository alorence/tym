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

set(TYM_WIN_RC logos/tym.rc)

# Set the list of translations (*.ts) files to compile them in src directory
file(GLOB TRANSLATIONS_FILES translations/*.ts)

# Compile translations (*.qm) files from *.ts
qt5_add_translation(TYM_TRANSLATIONS ${TRANSLATIONS_FILES})

# Write resources into proper cpp source file
qt5_add_resources(CPP_RESOURCES resources.qrc)

add_library(tym_resources STATIC ${TRANSLATIONS_FILES} ${CPP_RESOURCES} ${TYM_WIN_RC})

qt5_use_modules(tym_resources Core)