#include "catch.hpp"
#include "ashe/version.h"

using namespace ashe;

// Test: Version compare
//
TEST_CASE("VersionCompareTest") {
    Version v1("1.2.3.4");
    Version v2("4.3.2");
    Version v3(L"4.3.2.1");
    Version v4 = v3;

    REQUIRE(v1.isValid());
    REQUIRE(v2.isValid());
    REQUIRE(v1.isSameFormat(v2) == false);
    REQUIRE(v2.isSameFormat(v1) == false);

    REQUIRE((v1 != v3));
    REQUIRE((v3 == v4));
    REQUIRE((v3 >= v4));
    REQUIRE((v3 <= v4));
    REQUIRE((v1 <= v4));
    REQUIRE((v1 < v4));
    REQUIRE((v4 >= v1));
    REQUIRE((v4 > v1));
}
