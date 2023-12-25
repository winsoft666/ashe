#include "catch.hpp"
#include "ashe/all.hpp"
#include "ashe/os_ver.hpp"
#include "ashe/process_util.hpp"

#ifdef ASHE_WIN
TEST_CASE("PathUtil-ExpandEnvString", "") {
    std::wstring expand = ashe::PathUtil::ExpandEnvString(L"%ProgramFiles%", true);
    CHECK(expand == L"C:\\Program Files");

    expand = ashe::PathUtil::ExpandEnvString(L"%ProgramFiles%", false);
    if (ashe::OSVersion::IsWin64()) {
        bool is32Bit = false;
        bool ret = ashe::ProcessUtil::Is32BitProcess(GetCurrentProcess(), is32Bit);
        REQUIRE(ret);

        if (is32Bit)
            CHECK(expand == L"C:\\Program Files (x86)");
        else
            CHECK(expand == L"C:\\Program Files");
    }
}
#endif