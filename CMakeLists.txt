#################################################################################
#    C++ Common Library
#    ---------------------------------------------------------------------------
#    Copyright (C) 2020~2024 winsoft666 <winsoft666@outlook.com>.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#################################################################################

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")

project(ashe 
	VERSION 2.4
	LANGUAGES CXX
)

configure_file("${CMAKE_SOURCE_DIR}/include/ashe/config.h.in" "${CMAKE_SOURCE_DIR}/include/ashe/config.h")

option(ASHE_BUILD_TESTS "Build tests project" ON)
option(ASHE_USE_STATIC_CRT "Set to ON to build with static CRT on Windows (/MT)." OFF)
option(ASHE_BUILD_SHARED_LIBS "Set to ON to build dynamic library." OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (CMAKE_CXX_STANDARD 11)

if (MSVC AND ASHE_USE_STATIC_CRT)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


add_subdirectory(src)

if(ASHE_BUILD_TESTS)
    add_subdirectory(test)
endif()