#include "catch.hpp"
#include "ashe/os_version.h"

TEST_CASE("OsVersionTest") {
    using namespace ashe;
    bool w64 = OSVersion::IsWin64();
}