#include "catch.hpp"
#include "ashe/win_virtual_key.h"

#ifdef ASHE_WIN
// Test: Windows virtual key convert.
TEST_CASE("WinVirtualKeyProcessTest") {
    REQUIRE(ashe::WinVirtualKey::ToInteger("VK_SHIFT") == 0x10);
    REQUIRE(ashe::WinVirtualKey::ToInteger("VK_EXECUTE") == 0x2B);
    REQUIRE(ashe::WinVirtualKey::ToInteger("VK_RETURN") == 0x0D);

    REQUIRE(ashe::WinVirtualKey::ToString(0x0D) == "VK_RETURN");
    REQUIRE(ashe::WinVirtualKey::ToString(0x2B) == "VK_EXECUTE");
}
#endif