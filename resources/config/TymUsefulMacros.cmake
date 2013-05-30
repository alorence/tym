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

# Print all cmake variables
macro(print_all_variables)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endmacro(print_all_variables)

# Print content of _inc_dir variable
macro(print_include_dirs)
    get_directory_property(_incs INCLUDE_DIRECTORIES)
    message(STATUS "Include directories for ${CMAKE_CURRENT_SOURCE_DIR} :")
    foreach (_inc_dir ${_incs})
        message(STATUS "  * " ${_inc_dir})
    endforeach()
endmacro(print_include_dirs)

macro(today _result)
    if(WIN32)
        execute_process(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${_result})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\3\\2\\1" ${_result} ${${_result}})
    elseif(UNIX)
        execute_process(COMMAND "date" "+%d/%m/%Y" OUTPUT_VARIABLE ${_result})
        string(REGEX REPLACE "(..)/(..)/(....).*" "\\3\\2\\1" ${_result} ${${_result}})
    else(WIN32)
        message(SEND_ERROR "date not implemented")
        set(${_result} 00000000)
    endif(WIN32)
endmacro(today)

macro(tym_qt5_use_modules _target _link_type)
    if (NOT TARGET ${_target})
        message(FATAL_ERROR "The first argument to qt5_use_modules must be an existing target.")
    endif()
    if ("${_link_type}" STREQUAL "LINK_PUBLIC" OR "${_link_type}" STREQUAL "LINK_PRIVATE" )
        set(_qt5_modules ${ARGN})
        set(_qt5_link_type ${_link_type})
    else()
        set(_qt5_modules ${_link_type} ${ARGN})
    endif()

    if ("${_qt5_modules}" STREQUAL "")
        message(FATAL_ERROR "qt5_use_modules requires at least one Qt module to use.")
    endif()

    foreach(_module ${_qt5_modules})
        if (NOT Qt5${_module}_FOUND)
            find_package(Qt5${_module} PATHS ${_qt5_corelib_install_prefix} NO_DEFAULT_PATH)
            if (NOT Qt5${_module}_FOUND)
                message(FATAL_ERROR "Can not use \"${_module}\" module which has not yet been found.")
            endif()
        endif()
        get_target_property(_trgt_type ${_target} TYPE)
        if(NOT ${_trgt_type} STREQUAL "OBJECT_LIBRARY")
            target_link_libraries(${_target} ${_qt5_link_type} ${Qt5${_module}_LIBRARIES})
        endif()
        set_property(TARGET ${_target} APPEND PROPERTY INCLUDE_DIRECTORIES ${Qt5${_module}_INCLUDE_DIRS})
        set_property(TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS ${Qt5${_module}_COMPILE_DEFINITIONS})
        set_property(TARGET ${_target} APPEND PROPERTY COMPILE_DEFINITIONS_RELEASE QT_NO_DEBUG)

        if (Qt5_POSITION_INDEPENDENT_CODE)
            set_property(TARGET ${_target} PROPERTY POSITION_INDEPENDENT_CODE ${Qt5_POSITION_INDEPENDENT_CODE})
        endif()
    endforeach()
endmacro()
