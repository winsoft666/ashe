#include "catch.hpp"
#include "ashe/eastl.h"

TEST_CASE("EASTL-string") {
    ashe::string str = "123,456,789";
    str.split();
}