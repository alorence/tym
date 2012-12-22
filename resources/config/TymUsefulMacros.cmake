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