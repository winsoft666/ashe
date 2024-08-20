#include "catch.hpp"
#include "ashe/path_util.h"
#include "ashe/os_version.h"
#include "ashe/process_util.h"

using namespace ashe;

#ifdef ASHE_WIN
TEST_CASE("PathUtil-ExpandEnvString", "") {
    std::wstring expand = ashe::ExpandEnvString(L"%ProgramFiles%", true);
    CHECK(expand == L"C:\\Program Files");

    expand = ashe::ExpandEnvString(L"%ProgramFiles%", false);
    if (ashe::IsWin64()) {
        bool is32Bit = ashe::Is32BitProcess(GetCurrentProcess());

        if (is32Bit)
            CHECK(expand == L"C:\\Program Files (x86)");
        else
            CHECK(expand == L"C:\\Program Files");
    }
}

#endif

TEST_CASE("PathUtil-PathGetDirectory", "") {
    REQUIRE(PathGetDirectory(L"C:\\Program Files (x86)\\a b\\u_1 2.txt") == L"C:\\Program Files (x86)\\a b\\");
    REQUIRE(PathGetDirectory(L"C:\\Program Files (x86)\\a b\\u_1 2.txt", 2) == L"C:\\Program Files (x86)\\");
    REQUIRE(PathGetDirectory(L"C:\\Program Files (x86)\\a b\\u_1 2.txt", 3) == L"C:\\");
    REQUIRE(PathGetDirectory(L"C:\\Program Files (x86)\\a b\\u_1 2.txt", 50) == L"C:\\");

    REQUIRE(PathGetDirectory(L"C:\\Program Files (x86)\\a b\\u_1 2") == L"C:\\Program Files (x86)\\a b\\");
    REQUIRE(PathGetDirectory(L"C:\\Program Files (x86)\\a b\\u_1 2", 2) == L"C:\\Program Files (x86)\\");
    REQUIRE(PathGetDirectory(L"C:\\Program Files (x86)\\a b\\u_1 2", 3) == L"C:\\");
    REQUIRE(PathGetDirectory(L"C:\\Program Files (x86)\\a b\\u_1 2", 50) == L"C:\\");

    REQUIRE(PathGetDirectory(L"C:") == L"C:");
    REQUIRE(PathGetDirectory(L"C:", 50) == L"C:");

    REQUIRE(PathGetDirectory(L"C:\\") == L"C:\\");
    REQUIRE(PathGetDirectory(L"C:\\", 50) == L"C:\\");

    REQUIRE(PathGetDirectory(L"C:\\\\") == L"C:\\");
    REQUIRE(PathGetDirectory(L"C:\\\\", 50) == L"C:\\");
}

TEST_CASE("PathUtil-PathGetFileName", "") {
    REQUIRE(PathGetFileName(L"C:\\Program Files (x86)\\a b\\u_1 2.txt") == L"u_1 2.txt");
    REQUIRE(PathGetFileName(L"C:\\Program Files (x86)\\a b\\u_1 2") == L"u_1 2");
    REQUIRE(PathGetFileName(L"C:\\Program Files (x86)\\a b\\u_1 2.data.txt") == L"u_1 2.data.txt");
    REQUIRE(PathGetFileName(L"C:\\Program Files (x86)\\a b\\u_1 2.") == L"u_1 2.");

    REQUIRE(PathGetFileName(L"u_1 2.") == L"u_1 2.");
}

TEST_CASE("PathUtil-PathJoin", "") {
    REQUIRE(PathJoin(L"C:\\Program Files (x86)\\a b", L"c d\\q.txt") == L"C:\\Program Files (x86)\\a b\\c d\\q.txt");
    REQUIRE(PathJoin(L"C:\\Program Files (x86)\\a b\\", L"\\c d\\q.txt") == L"C:\\Program Files (x86)\\a b\\c d\\q.txt");
    REQUIRE(PathJoin(L"C:\\Program Files (x86)\\a b", L"\\c d\\q.txt") == L"C:\\Program Files (x86)\\a b\\c d\\q.txt");
    REQUIRE(PathJoin(L"C:\\Program Files (x86)\\a b/", L"\\c d\\q.txt") == L"C:\\Program Files (x86)\\a b/c d\\q.txt");
    REQUIRE(PathJoin(L"C:\\Program Files (x86)\\a b", L"") == L"C:\\Program Files (x86)\\a b");
    REQUIRE(PathJoin(L"", L"c d\\q.txt") == L"c d\\q.txt");
}