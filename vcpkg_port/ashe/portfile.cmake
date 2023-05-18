vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO winsoft666/ashe
    HEAD_REF master
    AUTHORIZATION_TOKEN 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
)

string(COMPARE EQUAL "${VCPKG_LIBRARY_LINKAGE}" "static" ASHE_STATIC)
string(COMPARE EQUAL "${VCPKG_CRT_LINKAGE}" "static" ASHE_USE_STATIC_CRT)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DASHE_STATIC:BOOL=${ASHE_STATIC}
        -DASHE_USE_STATIC_CRT:BOOL=${ASHE_USE_STATIC_CRT}
        -DBUILD_TESTS:BOOL=OFF
)

vcpkg_cmake_install()

if(EXISTS "${CURRENT_PACKAGES_DIR}/lib/cmake/ashe")
    vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/ashe)
elseif(EXISTS "${CURRENT_PACKAGES_DIR}/share/ashe")
    vcpkg_cmake_config_fixup(CONFIG_PATH share/ashe)
endif()

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")

vcpkg_copy_pdbs()
