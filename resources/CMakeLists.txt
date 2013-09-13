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

# RESOURCES ===================================================================
qt5_add_resources(TYM_RESOURCES_GENERATED resources.qrc)
# Store path of the generated cpp, as it must be compiled against the final executable
set(TYM_RESOURCES_GENERATED ${TYM_RESOURCES_GENERATED} PARENT_SCOPE)

if(MSVC)
    set(WIN_ICON_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/logos/tym.rc PARENT_SCOPE)
else(APPLE)
    set(OSX_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/logos/tym.icns PARENT_SCOPE)
endif()

add_library(TymResources STATIC ${TYM_RESOURCES_GENERATED})
qt5_use_modules(TymResources Core)

set(TYM_LOGO ${CMAKE_CURRENT_SOURCE_DIR}/logos/tym_logo.png PARENT_SCOPE)
set(TYM_LOGO_MINI ${CMAKE_CURRENT_SOURCE_DIR}/logos/tym_logo_mini.png PARENT_SCOPE)

# TRANSLATIONS ================================================================
# Only set a global variable with a list of all .ts files. qt5_add_translation or
# qt5_create_translation cannot be applied here, since cmake break with an error if
# .qm files are in the list of input files for an OBJECT library

# Set the list of translations (*.ts) files to compile them in src directory
file(GLOB TRANSLATIONS_FILES translations/*.ts)
set(TRANSLATION_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/translations/template.ts" PARENT_SCOPE)
set(TRANSLATIONS_FILES ${TRANSLATIONS_FILES} PARENT_SCOPE)
