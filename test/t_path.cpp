#include "catch.hpp"
#include "ashe/path.h"

using namespace ashe;

TEST_CASE("PathTest1", "") {
    Path p1 = "C:\\test";
    Path p2 = L"readme.md";
    Path p3 = p1 + p2;
    std::wstring p3W = static_cast<std::wstring>(p3);
    std::string p3A = static_cast<std::string>(p3);
    REQUIRE(p3W == L"C:\\test\\readme.md");
    REQUIRE(p3A == "C:\\test\\readme.md");

    p1 += p2;
    std::wstring p1W = static_cast<std::wstring>(p3);
    std::string p1A = static_cast<std::string>(p3);
    REQUIRE(p1W == L"C:\\test\\readme.md");
    REQUIRE(p1A == "C:\\test\\readme.md");
}