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

file(GLOB_RECURSE HEADERS ../include/*.h)
file(GLOB_RECURSE SOURCES ./*.cpp)

if(ASHE_BUILD_SHARED_LIBS)
	add_library(ashe SHARED ${HEADERS} ${SOURCES})

	target_compile_definitions(ashe PRIVATE ASHE_EXPORTS)
else()
	add_library(ashe STATIC ${HEADERS} ${SOURCES})

	target_compile_definitions(ashe PUBLIC ASHE_STATIC)
endif()

# set output name
set_target_properties(ashe PROPERTIES 
	OUTPUT_NAME $<IF:$<BOOL:${ASHE_BUILD_SHARED_LIBS}>,ashe,ashe-static>
	DEBUG_OUTPUT_NAME $<IF:$<BOOL:${ASHE_BUILD_SHARED_LIBS}>,ashe-d,ashe-static-d>)

if (MSVC)
  target_compile_definitions(ashe PRIVATE 
  	_WIN32 UNICODE _UNICODE NOMINMAX)
endif()


target_include_directories(ashe
	PUBLIC 
	"$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

if(WIN32)
	set_target_properties(ashe PROPERTIES COMPILE_FLAGS "/wd4251")
	target_compile_options(ashe PUBLIC "/Zc:__cplusplus")

	if(ASHE_BUILD_SHARED_LIBS)
		target_link_libraries(ashe PRIVATE 
			Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib Psapi.lib Shlwapi.lib)
	else()
		target_link_libraries(ashe PUBLIC 
			Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib Psapi.lib Shlwapi.lib)
	endif()
endif()

install(TARGETS ashe
	EXPORT asheConfig
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ../include/ashe DESTINATION include)

install(EXPORT asheConfig
    NAMESPACE ashe::
    DESTINATION share/ashe
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    asheConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/asheConfigVersion.cmake
        DESTINATION share/ashe)

install(DIRECTORY ../include/ashe 	DESTINATION include)