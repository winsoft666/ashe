#include "catch.hpp"
#include "ashe/uuid.h"
#include <map>

// Test: uuid generator.
//
TEST_CASE("UUIDTest") {
    std::map<std::string, int> values;
    const int totalNum = 100000;

    for (int i = 0; i < totalNum; i++) {
        values[ashe::CreateUuid()] = i;
    }

    REQUIRE(values.size() == totalNum);
}
