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


configure_file("${PROJECT_SOURCE_DIR}/include/ashe/config.hpp.in" "${PROJECT_SOURCE_DIR}/include/ashe/config.hpp")


set(DEBUG_LIBNAME_SUFFIX "-d" CACHE STRING "Optional suffix to append to the library name for a debug build")
mark_as_advanced(DEBUG_LIBNAME_SUFFIX)

include_directories(
  ../include
)

if (MSVC)
  add_definitions(
    -D_WIN32
  )
endif()

if (MSVC AND ASHE_USE_STATIC_CRT)
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

file(GLOB_RECURSE HEADER_FILES 			../include/*.hpp ../include/*.h)
file(GLOB_RECURSE SOURCE_FILES 			../src/*.cc ../src/*.cpp)

group_sources(${HEADER_FILES} ${SOURCE_FILES})

if(ASHE_BUILD_SHARED_LIBS)
	add_definitions(-DASHE_EXPORTS)
	
	add_library(
		${ASHE_LIB_NAME}
		SHARED
		${HEADER_FILES}
		${SOURCE_FILES}
	)
	
	# set output name prefix/suffix
	set_target_properties(${ASHE_LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${ASHE_LIB_NAME}
		DEBUG_OUTPUT_NAME ${ASHE_LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
else()
	add_definitions(-DASHE_STATIC)
	
	add_library(
		${ASHE_LIB_NAME}
		STATIC
		${HEADER_FILES}
		${SOURCE_FILES}
	)
	
	# set output name prefix/suffix
  set_target_properties(${ASHE_LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${ASHE_LIB_NAME}
		DEBUG_OUTPUT_NAME ${ASHE_LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
		
	set_target_properties(${ASHE_LIB_NAME} PROPERTIES 
		INTERFACE_COMPILE_DEFINITIONS ASHE_STATIC)
endif()

set_target_properties(${ASHE_LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if(WIN32)
  set_target_properties(${ASHE_LIB_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")

  target_link_libraries(${ASHE_LIB_NAME} PUBLIC 
	  Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib Psapi.lib Shlwapi.lib)
endif()

install(DIRECTORY ../include 	DESTINATION include)

install(TARGETS ${ASHE_LIB_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

