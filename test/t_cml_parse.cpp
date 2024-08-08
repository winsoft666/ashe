#include "catch.hpp"
#include "ashe/cmd_line_parser.h"

// Test: command line parser.
//
TEST_CASE("CommandLineParseTest") {
    std::wstring wparam = L"\"C:\\Program Files (x86)\\Google\\Chrome.exe\" -k1=v1 -k2:v2 /k3=v3 /k4:v4 /k5 -k6=v6= /k7=\"v7 /v=-'\"";
    ashe::CmdLineParser clp(wparam);
    printf("Key-Value list:\n");
    for (ashe::CmdLineParser::ITERPOS it = clp.begin(); it != clp.end(); ++it)
        printf("Key:%ls, Value:%ls\n", it->first.c_str(), it->second.c_str());
    printf("\n");

    REQUIRE(clp.getVal(L"k1") == L"v1");
    REQUIRE(clp.getVal(L"k2") == L"v2");
    REQUIRE(clp.getVal(L"k3") == L"v3");
    REQUIRE(clp.getVal(L"k4") == L"v4");
    REQUIRE(clp.getVal(L"k5") == L"");
    REQUIRE(clp.getVal(L"k6") == L"v6=");
    REQUIRE(clp.getVal(L"k7") == L"v7 /v=-'");
}