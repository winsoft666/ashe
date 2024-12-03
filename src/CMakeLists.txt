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

file(GLOB WIN_HEADERS LIST_DIRECTORIES false ../include/ashe/win/*.h)
file(GLOB MESSAGE_LOOP_HEADERS LIST_DIRECTORIES false ../include/ashe/message_loop/*.h)
file(GLOB HEADERS LIST_DIRECTORIES false ../include/ashe/*.h)

file(GLOB WIN_SOURCES LIST_DIRECTORIES false ./win/*.cpp)
file(GLOB MESSAGE_LOOP_SOURCES LIST_DIRECTORIES false ./message_loop/*.cpp)
file(GLOB SOURCES LIST_DIRECTORIES false ./*.cpp)

source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})

source_group("Header Files\\message_loop" FILES ${MESSAGE_LOOP_HEADERS})
source_group("Source Files\\message_loop" FILES ${MESSAGE_LOOP_SOURCES})

if(WIN32)
	source_group("Header Files\\win" FILES ${WIN_HEADERS})
	source_group("Source Files\\win" FILES ${WIN_SOURCES})
endif()

if(ASHE_BUILD_SHARED_LIBS)
	if(WIN32)
		add_library(ashe SHARED 
			${HEADERS} 
			${WIN_HEADERS} 
			${SOURCES} 
			${WIN_SOURCES} 
			${MESSAGE_LOOP_HEADERS} 
			${MESSAGE_LOOP_SOURCES}
		)
	else()
		add_library(ashe SHARED 
			${HEADERS} 
			${SOURCES} 
			${MESSAGE_LOOP_HEADERS} 
			${MESSAGE_LOOP_SOURCES}
		)
	endif()
	target_compile_definitions(ashe PRIVATE ASHE_EXPORTS)
else()
	if(WIN32)
		add_library(ashe STATIC 
			${HEADERS} 
			${WIN_HEADERS} 
			${SOURCES} 
			${WIN_SOURCES} 
			${MESSAGE_LOOP_HEADERS} 
			${MESSAGE_LOOP_SOURCES}
		)
	else()
		add_library(ashe STATIC 
			${HEADERS} 
			${SOURCES} 
			${MESSAGE_LOOP_HEADERS} 
			${MESSAGE_LOOP_SOURCES}
		)
	endif()

	target_compile_definitions(ashe PUBLIC ASHE_STATIC)
endif()

if(ASHE_USE_STATIC_CRT)
	target_compile_definitions(ashe PUBLIC ASHE_USE_STATIC_CRT)
endif()

# set output name
set_target_properties(ashe PROPERTIES 
	OUTPUT_NAME $<IF:$<BOOL:${ASHE_BUILD_SHARED_LIBS}>,ashe,ashe-static>
	DEBUG_OUTPUT_NAME $<IF:$<BOOL:${ASHE_BUILD_SHARED_LIBS}>,ashe-d,ashe-static-d>)

if (WIN32)
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

	set(WIN_LIBS Ws2_32.lib Iphlpapi.lib Userenv.lib Dbghelp.lib 
		Psapi.lib Shlwapi.lib Version.lib taskschd.lib 
		Shell32.lib wintrust.lib crypt32.lib ole32.lib oleaut32.lib
		winhttp.lib
	)

	if(ASHE_BUILD_SHARED_LIBS)
		target_link_libraries(ashe PRIVATE ${WIN_LIBS})
	else()
		target_link_libraries(ashe PUBLIC ${WIN_LIBS})
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