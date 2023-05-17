#include "catch.hpp"
#include "ashe/optional.hpp"
#include <string>

TEST_CASE("Optional1", "") {
    ashe::optional<int> emptyInt = 100;
    CHECK(emptyInt);
    CHECK(emptyInt.value() == 100);

    emptyInt = ashe::nullopt;
    CHECK(!emptyInt);

    ashe::optional<std::string> emptyStr = ashe::nullopt;
    CHECK(emptyStr.value_or("test") == "test");
}