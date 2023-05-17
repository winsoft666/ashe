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

set (CMAKE_CXX_STANDARD 14)

set(EXE_NAME ashe-test)

file(GLOB ASHE_HEADER_FILES ../include/ashe.hpp)
file(GLOB TEST_SOURCE_FILES ./*.cpp)

if (ASHE_NOT_HEADER_ONLY)
	source_group("ashe" FILES ${ASHE_HEADER_FILES})
else()
	source_group("ashe" FILES ${ASHE_HEADER_FILES} ${ASHE_IMPL_FILES})
endif()
source_group("test" FILES ${TEST_SOURCE_FILES})

include_directories(../include)

if (ASHE_NOT_HEADER_ONLY)
	add_executable(
		${EXE_NAME}
		${ASHE_HEADER_FILES}
		${TEST_SOURCE_FILES})
	
	add_dependencies(${EXE_NAME} ${LIB_NAME})
	target_link_libraries(${EXE_NAME} ${LIB_NAME})
else()
	add_executable(
		${EXE_NAME}
		${ASHE_HEADER_FILES} ${ASHE_IMPL_FILES}
		${TEST_SOURCE_FILES})
endif()

# Win32 Console
if (WIN32 OR _WIN32)
	set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
	set_target_properties(${EXE_NAME} PROPERTIES COMPILE_DEFINITIONS "_CONSOLE")
else()
   target_link_libraries(${EXE_NAME} pthread)
endif()
