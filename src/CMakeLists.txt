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

configure_file(version.h.in version.h)

# In source files, all includes are relative to src (in cmlake source or binary directory)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(concretetasks)
add_subdirectory(dbaccess)
add_subdirectory(network)
add_subdirectory(gui)
add_subdirectory(tools)
add_subdirectory(widgets)
add_subdirectory(wizards)

# These headers doesn't define QObject children,
# they does not need to be parsed with moc tool.
# They are added to the source list to appear in
# IDE project (QtCreator, Xcode, etc.)
set(BASIC_HDRS
    version.h.in
    commons.h
)

set(SOURCES
    main.cpp
)

# Generate template.qm in another folder, to prevent its inclusion in a deliverable
set_source_files_properties(${TRANSLATION_TEMPLATE} PROPERTIES OUTPUT_LOCATION ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY})
# Used to localize the resulting *.qm files when building deliverables
set(TRANSLATIONS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)
if(TYM_UPDATE_TRANSLATIONS)
    message(STATUS "*********************************************************************")
    message(STATUS "* TS files will be updated. To prevent their deletion, please don't *")
    message(STATUS "* forget to set TYM_UPDATE_TRANSLATIONS to OFF                      *")
    message(STATUS "*********************************************************************")
    qt5_create_translation(TYM_TRANSLATIONS ${CMAKE_CURRENT_SOURCE_DIR} ${TRANSLATIONS_FILES})
else()
    qt5_add_translation(TYM_TRANSLATIONS ${TRANSLATIONS_FILES})
endif()

if(WIN32)
    set(EXECUTABLE_OPTION WIN32)
elseif(APPLE)
    set(EXECUTABLE_OPTION MACOSX_BUNDLE)
    # CMake will copy icon to Resource folder at build time
    set_source_files_properties(${OSX_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    list(APPEND SOURCES ${OSX_ICON_FILE})
endif()

# Configure final executable building
add_executable(${TYM_FINAL_TARGET}
    ${EXECUTABLE_OPTION}
    ${SOURCES}
    ${BASIC_HDRS}
    ${TYM_TRANSLATIONS}
    ${TRANSLATIONS_FILES}
    $<TARGET_OBJECTS:TymConcretetasks>
    $<TARGET_OBJECTS:TymDbaccess>
    $<TARGET_OBJECTS:TymGui>
    $<TARGET_OBJECTS:TymNetwork>
    $<TARGET_OBJECTS:TymTools>
    $<TARGET_OBJECTS:TymWidgets>
    $<TARGET_OBJECTS:TymWizards>
    $<TARGET_OBJECTS:TymResources>
)

target_link_libraries(${TYM_FINAL_TARGET} Logger)

# Add Qt5 modules dependencies
qt5_use_modules(${TYM_FINAL_TARGET}
    Core
    Widgets
    Sql
    Network
)

if(MSVC)
    # Link with Qt WinMain, ensuring that the entry function will have
    # signature required by MSVC
    target_link_libraries(${TYM_FINAL_TARGET} Qt5::WinMain)
elseif(APPLE)
    set_target_properties(${TYM_FINAL_TARGET} PROPERTIES OUTPUT_NAME ${TYM_NAME})
    # configure some properties on Info.plist
    set_target_properties(${TYM_FINAL_TARGET} PROPERTIES MACOSX_BUNDLE_ICON_FILE tym.icns)
    set_target_properties(${TYM_FINAL_TARGET} PROPERTIES MACOSX_BUNDLE_INFO_STRING "Tag Your Music bundle")
    set_target_properties(${TYM_FINAL_TARGET} PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.tym.gui")
    set_target_properties(${TYM_FINAL_TARGET} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING ${TYM_VERSION})
    set_target_properties(${TYM_FINAL_TARGET} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${TYM_NAME})
    set_target_properties(${TYM_FINAL_TARGET} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING ${TYM_SHORT_VERSION})
    set_target_properties(${TYM_FINAL_TARGET} PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION ${TYM_VERSION})
    set_target_properties(${TYM_FINAL_TARGET} PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright 2012-2013 Antoine Lorence")
endif()

if(NOT APPLE)
    install(TARGETS ${TYM_FINAL_TARGET} RUNTIME DESTINATION bin)
endif()
