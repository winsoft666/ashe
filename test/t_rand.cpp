#include "catch.hpp"
#include "ashe/rand_util.h"
#include <set>

using namespace ashe;

TEST_CASE("RandTest-uint64_t", "") {
    std::set<uint64_t> m;

    for (int i = 0; i < 10000; i++) {
        m.insert(RandUint64());
    }

    REQUIRE(m.size() == 10000);
}

TEST_CASE("RandTest-int-1", "") {
    std::set<int> m;

    for (int i = 0; i < 10000; i++) {
        m.insert(RandInt(-10000, 10000));
    }

    REQUIRE(m.size() > 5000);

    for (std::set<int>::iterator it = m.begin(); it != m.end(); it++) {
        int i = *it;
        REQUIRE((i >= -10000 && i <= 10000));
    }
}

TEST_CASE("RandTest-int-2", "") {
    std::set<int> m;

    for (int i = 0; i < 10000; i++) {
        m.insert(RandInt(10000, 20000));
    }

    REQUIRE(m.size() > 5000);

    for (std::set<int>::iterator it = m.begin(); it != m.end(); it++) {
        int i = *it;
        REQUIRE((i >= 10000 && i <= 20000));
    }
}

TEST_CASE("RandTest-double", "") {
    std::set<double> m;

    for (int i = 0; i < 10000; i++) {
        m.insert(RandDouble());
    }

    REQUIRE(m.size() > 5000);

    for (std::set<double>::iterator it = m.begin(); it != m.end(); it++) {
        int i = *it;
        REQUIRE((i >= 0 && i < 1));
    }
}

TEST_CASE("RandTest-string", "") {
    std::set<std::string> m;

    for (int i = 0; i < 10000; i++) {
        m.insert(RandBytesAsString(10));
    }

    REQUIRE(m.size() == 10000);

    for (std::set<std::string>::iterator it = m.begin(); it != m.end(); it++) {
        std::string i = *it;
        REQUIRE(i.size() == 10);
    }
}