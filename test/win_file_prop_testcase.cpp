#include "catch.hpp"
#include "ashe/win_file_property.h"
#include "ashe/process_util.h"

#ifdef ASHE_WIN
TEST_CASE("WinFilePropTest") {
    ashe::WinFileProperty fileProp;
    REQUIRE(true == fileProp.open(L"C:\\depends22_x86\\depends.exe"));
}
#endif