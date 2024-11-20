#include "catch.hpp"
#include "ashe/scoped_object.h"

using namespace ashe;

#ifdef ASHE_WIN
TEST_CASE("ScopedObject", "") {
    ScopedComInitialize com;
}
#endif