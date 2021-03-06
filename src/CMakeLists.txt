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

set(BEATPORT_API_KEY CACHE STRING "Beatport API key to use. You can obtain it for your fork at https://accounts.beatport.com/developer/request-api-key")
set(BEATPORT_API_SECRET CACHE STRING "Beatport API secret to use")
if("${BEATPORT_API_KEY}" STREQUAL "" OR "${BEATPORT_API_SECRET}" STREQUAL "")
    message(WARNING "Variables BEATPORT_API_KEY and BEATPORT_API_SECRET must "
        "be set with valid keys in order to obtain a working software. "
        "Beatport now use OAuth authentication for all search through its API. "
        "Please visit https://accounts.beatport.com/developer/request-api-key "
        "to obtain a key for your fork of ${TYM_NAME}")
endif()
configure_file(config.h.in config.h)

# In source files, all includes are relative to src (in cmake source or binary directory)
# These 2 folders must be in the include list for all submodules
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(tasks)
add_subdirectory(dbaccess)
add_subdirectory(network)
add_subdirectory(widgets)
add_subdirectory(gui)
add_subdirectory(tools)

# These headers doesn't define QObject children,
# they does not need to be parsed with moc tool.
# They are added to the source list to appear in
# IDE project (QtCreator, Xcode, etc.)
set(BASIC_HDRS
    config.h.in
    commons.h
    interfaces/taskfactory.h
    interfaces/genericsingleton.h
)

set(SOURCES
    main.cpp
)

if(WIN32)
    set(EXECUTABLE_OPTION
        WIN32
        ${WIN_ICON_RESOURCE}
    )
elseif(APPLE)
    set(EXECUTABLE_OPTION MACOSX_BUNDLE)
    # CMake will copy icon to Resource folder at build time
    set_source_files_properties(${OSX_ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    list(APPEND SOURCES ${OSX_ICON_FILE})
endif()

# Configure final executable building
add_executable(${TYM_TARGET}
    ${EXECUTABLE_OPTION}
    ${SOURCES}
    ${BASIC_HDRS}
    ${TYM_RESOURCES_GENERATED}
)

# Mark cpp file as generated when TymResources target will be built
set_source_files_properties(${TYM_RESOURCES_GENERATED} PROPERTIES GENERATED true)

# !! Order is important ! Gui depends on Widget, Widget depends on Tools, etc.
# The lib A depending on lib B must be BEFORE in the folowing list (http://stackoverflow.com/a/409470/1887976)
target_link_libraries(${TYM_TARGET}
    TymGui
    TymWidgets
    TymTools
    TymTasks
    TymNetwork
    TymDbaccess
    TymResources
    CuteLogger
    o2
)

# Add Qt5 modules dependencies
qt5_use_modules(${TYM_TARGET} Core Widgets Sql Network WebKitWidgets)

if(MSVC)
    # Link with Qt WinMain, ensuring that the entry function will have
    # signature required by MSVC
    target_link_libraries(${TYM_TARGET} Qt5::WinMain)
elseif(APPLE)
    set_target_properties(${TYM_TARGET} PROPERTIES OUTPUT_NAME ${TYM_NAME})
    # configure some properties on Info.plist
    set_target_properties(${TYM_TARGET} PROPERTIES MACOSX_BUNDLE_ICON_FILE tym.icns)
    set_target_properties(${TYM_TARGET} PROPERTIES MACOSX_BUNDLE_INFO_STRING "Tag Your Music bundle")
    set_target_properties(${TYM_TARGET} PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.tym.gui")
    set_target_properties(${TYM_TARGET} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING ${TYM_VERSION})
    set_target_properties(${TYM_TARGET} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${TYM_NAME})
    set_target_properties(${TYM_TARGET} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING ${TYM_SHORT_VERSION})
    set_target_properties(${TYM_TARGET} PROPERTIES MACOSX_BUNDLE_BUNDLE_VERSION ${TYM_VERSION})
    set_target_properties(${TYM_TARGET} PROPERTIES MACOSX_BUNDLE_COPYRIGHT "Copyright 2012-2013 Antoine Lorence")
endif()

if(NOT APPLE)
    install(TARGETS ${TYM_TARGET} RUNTIME DESTINATION bin)
endif()
