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

# TRANSLATIONS ================================================================
file(GLOB TRANSLATIONS_FILES translations/*.ts)
file(GLOB TRANSLATION_TEMPLATE translations/template.ts)

# Generate template.qm in another folder, to prevent its inclusion in a deliverable
set_source_files_properties(${TRANSLATION_TEMPLATE} PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY})
if(TYM_UPDATE_TRANSLATIONS)
    message(STATUS "*********************************************************************")
    message(STATUS "* TS files will be updated. To prevent their deletion, please don't *")
    message(STATUS "* forget to set TYM_UPDATE_TRANSLATIONS to OFF                      *")
    message(STATUS "*********************************************************************")

    if(TYM_CLEAN_TRANSLATIONS)
        qt5_create_translation(TYM_TRANSLATIONS ${CMAKE_SOURCE_DIR}/src ${TRANSLATIONS_FILES} OPTIONS "-no-obsolete")
    else()
        qt5_create_translation(TYM_TRANSLATIONS ${CMAKE_SOURCE_DIR}/src ${TRANSLATIONS_FILES})
    endif()
else()
    qt5_add_translation(TYM_TRANSLATIONS ${TRANSLATIONS_FILES})
endif()

# RESOURCES ===================================================================
qt5_add_resources(TYM_RESOURCES_GENERATED resources.qrc)

if(MSVC)
    set(WIN_ICON_RESOURCE ${CMAKE_CURRENT_SOURCE_DIR}/logos/tym.rc PARENT_SCOPE)
else(APPLE)
    set(OSX_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/logos/tym.icns PARENT_SCOPE)
endif()

add_library(TymResources STATIC ${TYM_RESOURCES_GENERATED} ${TYM_TRANSLATIONS} ${TRANSLATIONS_FILES})
qt5_use_modules(TymResources Core)

# Expose some variables to other targets --------------------------------------

# Store path of the generated cpp, as it must be compiled against the final executable
set(TYM_RESOURCES_GENERATED ${TYM_RESOURCES_GENERATED} PARENT_SCOPE)
# Used to localize the resulting *.qm files when building deliverables
set(TRANSLATIONS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
set(TYM_LOGO ${CMAKE_CURRENT_SOURCE_DIR}/logos/tym_logo.png PARENT_SCOPE)
set(TYM_LOGO_MINI ${CMAKE_CURRENT_SOURCE_DIR}/logos/tym_logo_mini.png PARENT_SCOPE)
