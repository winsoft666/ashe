#include "catch.hpp"
#include "ashe/arch.h"
#ifdef ASHE_WIN
#include "ashe/win/persistent_uuid.h"

using namespace ashe;
using namespace ashe::win;

TEST_CASE("PersisitentUUIDTest", "") {
    std::wstring uuid;
    bool b = GetPersistentUUID(uuid);

    REQUIRE(b);
    REQUIRE(!uuid.empty());

    for (int i = 0; i < 1000; i++) {
        std::wstring str;
        b = GetPersistentUUID(str);

        REQUIRE(b);
        REQUIRE(str == uuid);
    }
}
#endif