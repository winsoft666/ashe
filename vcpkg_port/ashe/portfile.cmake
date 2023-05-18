vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO winsoft666/ashe
    HEAD_REF main
    AUTHORIZATION_TOKEN $ENV{GITHUB_TOKEN}
)

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "static" LIBRARY_LINKAGE_HAS_STATIC)
string(COMPARE EQUAL "${VCPKG_CRT_LINKAGE}" "static" ASHE_USE_STATIC_CRT)

if(${LIBRARY_LINKAGE_HAS_STATIC})
	set(ASHE_BUILD_SHARED_LIBS OFF)
else()
	set(ASHE_BUILD_SHARED_LIBS ON)
endif()

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DASHE_BUILD_SHARED_LIBS:BOOL=${ASHE_BUILD_SHARED_LIBS}
        -DASHE_USE_STATIC_CRT:BOOL=${ASHE_USE_STATIC_CRT}
        -DASHE_BUILD_TESTS:BOOL=OFF
)

vcpkg_cmake_install()

if(EXISTS "${CURRENT_PACKAGES_DIR}/lib/cmake/ashe")
    vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/ashe)
elseif(EXISTS "${CURRENT_PACKAGES_DIR}/share/ashe")
    vcpkg_cmake_config_fixup(CONFIG_PATH share/ashe)
endif()

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)

vcpkg_copy_pdbs()
