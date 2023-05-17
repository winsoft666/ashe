# Install script for directory: D:/CODE/ashe

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/ashe")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/CODE/ashe/build/lib/Debug/ashe-static-d.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/CODE/ashe/build/lib/Release/ashe-static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/CODE/ashe/build/lib/MinSizeRel/ashe-static.lib")
  elseif(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "D:/CODE/ashe/build/lib/RelWithDebInfo/ashe-static.lib")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES "D:/CODE/ashe/./include/ashe.hpp")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/ashe" TYPE FILE FILES
    "D:/CODE/ashe/./include/ashe.hpp"
    "D:/CODE/ashe/./include/ashe/allow_enum_flags.hpp"
    "D:/CODE/ashe/./include/ashe/arch.hpp"
    "D:/CODE/ashe/./include/ashe/base64.hpp"
    "D:/CODE/ashe/./include/ashe/buffer_queue.hpp"
    "D:/CODE/ashe/./include/ashe/byteorder.hpp"
    "D:/CODE/ashe/./include/ashe/cmd_line_parse.hpp"
    "D:/CODE/ashe/./include/ashe/config.hpp"
    "D:/CODE/ashe/./include/ashe/configor.hpp"
    "D:/CODE/ashe/./include/ashe/cpp_feature.hpp"
    "D:/CODE/ashe/./include/ashe/crc32.hpp"
    "D:/CODE/ashe/./include/ashe/enum_flags.hpp"
    "D:/CODE/ashe/./include/ashe/enum_flagsfwd.hpp"
    "D:/CODE/ashe/./include/ashe/enum_iterator.hpp"
    "D:/CODE/ashe/./include/ashe/event.hpp"
    "D:/CODE/ashe/./include/ashe/file.hpp"
    "D:/CODE/ashe/./include/ashe/filesystem.hpp"
    "D:/CODE/ashe/./include/ashe/hex_encode.hpp"
    "D:/CODE/ashe/./include/ashe/http_util.hpp"
    "D:/CODE/ashe/./include/ashe/ipaddress.hpp"
    "D:/CODE/ashe/./include/ashe/json.hpp"
    "D:/CODE/ashe/./include/ashe/logger.hpp"
    "D:/CODE/ashe/./include/ashe/macros.hpp"
    "D:/CODE/ashe/./include/ashe/md5.hpp"
    "D:/CODE/ashe/./include/ashe/memory_pool.hpp"
    "D:/CODE/ashe/./include/ashe/optional.hpp"
    "D:/CODE/ashe/./include/ashe/os_ver.hpp"
    "D:/CODE/ashe/./include/ashe/path_util.hpp"
    "D:/CODE/ashe/./include/ashe/process.hpp"
    "D:/CODE/ashe/./include/ashe/process_util.hpp"
    "D:/CODE/ashe/./include/ashe/scoped_object.hpp"
    "D:/CODE/ashe/./include/ashe/sha1.hpp"
    "D:/CODE/ashe/./include/ashe/sha256.hpp"
    "D:/CODE/ashe/./include/ashe/sha512.hpp"
    "D:/CODE/ashe/./include/ashe/singleton_class.hpp"
    "D:/CODE/ashe/./include/ashe/singleton_process.hpp"
    "D:/CODE/ashe/./include/ashe/string_encode.hpp"
    "D:/CODE/ashe/./include/ashe/string_helper.hpp"
    "D:/CODE/ashe/./include/ashe/thread.hpp"
    "D:/CODE/ashe/./include/ashe/thread_pool.hpp"
    "D:/CODE/ashe/./include/ashe/time_util.hpp"
    "D:/CODE/ashe/./include/ashe/timer.hpp"
    "D:/CODE/ashe/./include/ashe/trace.hpp"
    "D:/CODE/ashe/./include/ashe/url_encode.hpp"
    "D:/CODE/ashe/./include/ashe/uuid.hpp"
    "D:/CODE/ashe/./include/ashe/variant.hpp"
    "D:/CODE/ashe/./include/ashe/version.hpp"
    "D:/CODE/ashe/./include/ashe/win_criticalsection.hpp"
    "D:/CODE/ashe/./include/ashe/win_driver_info.hpp"
    "D:/CODE/ashe/./include/ashe/win_env_variable.hpp"
    "D:/CODE/ashe/./include/ashe/win_exception_catch.hpp"
    "D:/CODE/ashe/./include/ashe/win_http.hpp"
    "D:/CODE/ashe/./include/ashe/win_http_client.hpp"
    "D:/CODE/ashe/./include/ashe/win_hwnd.hpp"
    "D:/CODE/ashe/./include/ashe/win_ini.hpp"
    "D:/CODE/ashe/./include/ashe/win_process_finder.hpp"
    "D:/CODE/ashe/./include/ashe/win_registry.hpp"
    "D:/CODE/ashe/./include/ashe/win_schedule_task.hpp"
    "D:/CODE/ashe/./include/ashe/win_service_base.hpp"
    "D:/CODE/ashe/./include/ashe/win_service_helper.hpp"
    "D:/CODE/ashe/./include/ashe/win_shellink_creator.hpp"
    "D:/CODE/ashe/./include/ashe/win_shellink_parser.hpp"
    "D:/CODE/ashe/./include/ashe/win_timer.hpp"
    "D:/CODE/ashe/./include/ashe/win_virtual_key.hpp"
    "D:/CODE/ashe/./include/ashe/windows_lite.hpp"
    "D:/CODE/ashe/./include/ashe/xorstr.hpp"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("D:/CODE/ashe/build/3rd/spdlog-1.x/cmake_install.cmake")
  include("D:/CODE/ashe/build/test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "D:/CODE/ashe/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
