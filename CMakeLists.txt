#################################################################################
#    C++ Common Library
#    ---------------------------------------------------------------------------
#    Copyright (C) 2022 winsoft666 <winsoft666@outlook.com>.
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

cmake_minimum_required(VERSION 3.4 FATAL_ERROR)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake;${CMAKE_MODULE_PATH}")

project(ashe CXX)

function(group_sources)
  foreach(_source IN ITEMS ${ARGN})
  if (IS_ABSOLUTE "${_source}")
      file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
  else()
      set(_source_rel "${_source}")
  endif()
  get_filename_component(_source_path "${_source_rel}" PATH)
  string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
  source_group("${_source_path_msvc}" FILES "${_source}")
  endforeach()
endfunction(group_sources)


configure_file("${PROJECT_SOURCE_DIR}/include/ashe/config.hpp.in" "${PROJECT_SOURCE_DIR}/include/ashe/config.hpp")

option(BUILD_TESTS "Build tests project" ON)
option(USE_STATIC_CRT "Set to ON to build with static CRT on Windows (/MT)." OFF)


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Unicode Support
add_definitions(-DUNICODE -D_UNICODE -DNOMINMAX)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_CXX_STANDARD 11)

#########################################
# Add spdlog
set(SPDLOG_NO_EXCEPTIONS ON)
set(SPDLOG_WCHAR_FILENAMES ON)
if(WIN32)
  set(SPDLOG_WCHAR_SUPPORT ON)
endif()
add_subdirectory(./3rd/spdlog-1.x)

###########################################
set(DEBUG_LIBNAME_SUFFIX "-d" CACHE STRING "Optional suffix to append to the library name for a debug build")
mark_as_advanced(DEBUG_LIBNAME_SUFFIX)

include_directories(
  ./include
  ./include/ashe
  ./3rd/spdlog-1.x/include
)

if(ASHE_BUILD_SHARED_LIBS)
	set(LIB_NAME ashe)
	set(ASHE_STATIC ON)
else()
	set(LIB_NAME ashe-static)
	set(ASHE_STATIC OFF)
endif()

add_definitions(
  -DSPDLOG_NO_EXCEPTIONS
  -DSPDLOG_WCHAR_FILENAMES
)

if (MSVC)
  add_definitions(
    -D_WIN32
  )
endif()

if (MSVC AND USE_STATIC_CRT)
	set(CompilerFlags
		CMAKE_CXX_FLAGS
		CMAKE_CXX_FLAGS_DEBUG
		CMAKE_CXX_FLAGS_RELEASE
		CMAKE_C_FLAGS
		CMAKE_C_FLAGS_DEBUG
		CMAKE_C_FLAGS_RELEASE
		)
	foreach(CompilerFlag ${CompilerFlags})
		string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
	endforeach()
endif()

file(GLOB_RECURSE HEADER_FILES 			./include/*.hpp ./include/*.h)
file(GLOB_RECURSE SOURCE_FILES 			./src/*.cc ./src/*.cpp)

group_sources(${HEADER_FILES} ${SOURCE_FILES})

if(ASHE_BUILD_SHARED_LIBS)
	add_definitions(-DASHE_EXPORTS)
	
	add_library(
		${LIB_NAME}
		SHARED
		${HEADER_FILES}
		${SOURCE_FILES}
	)
	
	# set output name prefix/suffix
	set_target_properties(${LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${LIB_NAME}
		DEBUG_OUTPUT_NAME ${LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
else()
	add_definitions(-DASHE_STATIC)
	
	add_library(
		${LIB_NAME}
		STATIC
		${HEADER_FILES}
		${SOURCE_FILES}
	)
	
	# set output name prefix/suffix
  set_target_properties(${LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${LIB_NAME}
		DEBUG_OUTPUT_NAME ${LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
		
	set_target_properties(${LIB_NAME} PROPERTIES 
		INTERFACE_COMPILE_DEFINITIONS ASHE_STATIC)
endif()

set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if(WIN32)
  set_target_properties(${LIB_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")

  target_link_libraries(${LIB_NAME} PUBLIC 
	  Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib Psapi.lib Shlwapi.lib)
endif()

if(BUILD_TESTS)
    add_subdirectory(test)
    enable_testing()
endif()