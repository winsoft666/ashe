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

set (PACKAGE_VERSION 1.7)

file(GLOB_RECURSE HEADERS ../include/*.h)
file(GLOB_RECURSE SOURCES ./*.cpp)

if(ASHE_BUILD_SHARED_LIBS)
	add_library(
		${ASHE_LIB_NAME}
		SHARED
		${HEADERS}
		${SOURCES}
	)

	target_compile_definitions(${ASHE_LIB_NAME} PRIVATE ASHE_EXPORTS)
	
	# set output name prefix/suffix
	set_target_properties(${ASHE_LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${ASHE_LIB_NAME}
		DEBUG_OUTPUT_NAME ${ASHE_LIB_NAME}${DEBUG_LIBNAME_SUFFIX})
else()
	add_library(
		${ASHE_LIB_NAME}
		STATIC
		${HEADERS}
		${SOURCES}
	)

	target_compile_definitions(${ASHE_LIB_NAME} PUBLIC ASHE_STATIC)
	
	# set output name prefix/suffix
  	set_target_properties(${ASHE_LIB_NAME} PROPERTIES 
		OUTPUT_NAME ${ASHE_LIB_NAME} 
		DEBUG_OUTPUT_NAME ${ASHE_LIB_NAME}${DEBUG_LIBNAME_SUFFIX})

	set_target_properties(${ASHE_LIB_NAME} PROPERTIES 
		INTERFACE_COMPILE_DEFINITIONS ASHE_STATIC)
endif()

# include both source and headers in the files tab in Visual Studio
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/../include PREFIX "Header Files" FILES ${HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Source Files" FILES ${SOURCES})

if (MSVC)
  target_compile_definitions(${ASHE_LIB_NAME} PRIVATE _WIN32)
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

set_target_properties(${ASHE_LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(${ASHE_LIB_NAME} 
	PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

if(WIN32)
  set_target_properties(${ASHE_LIB_NAME} PROPERTIES COMPILE_FLAGS "/wd4251")

  target_link_libraries(${ASHE_LIB_NAME} PUBLIC 
	  Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib Psapi.lib Shlwapi.lib)
endif()

install(TARGETS ${ASHE_LIB_NAME}
	EXPORT ashe-targets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR}
	INCLUDES DESTINATION include
)

install(EXPORT ashe-targets
    NAMESPACE ashe::
    DESTINATION share/ashe
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ashe-config.cmake.in"
[[include("${CMAKE_CURRENT_LIST_DIR}/ashe-targets.cmake")
]])
configure_file("${CMAKE_CURRENT_BINARY_DIR}/ashe-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/ashe-config.cmake" @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ashe-config.cmake DESTINATION share/ashe)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    asheConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/asheConfigVersion.cmake
        DESTINATION share/ashe)

install(DIRECTORY ../include/ashe 	DESTINATION include)