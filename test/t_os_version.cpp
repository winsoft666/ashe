#include "catch.hpp"
#include "ashe/os_version.h"
#include "ashe/macros.h"

using namespace ashe;

#ifdef ASHE_WIN
TEST_CASE("OsVersionTest") {
    bool w64 = IsWin64();

    bool w11 = IsWindows11();
    bool w10p = IsWindows8OrHigher();
    bool wvistap = IsWindowsVistaOrHigher();
}
#endif