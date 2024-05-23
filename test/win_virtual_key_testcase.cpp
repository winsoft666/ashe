#include "catch.hpp"
#include "ashe/all.hpp"

#ifdef ASHE_WIN
// Test: Windows virtual key convert.
TEST_CASE("WinVirtualKeyProcessTest") {
    REQUIRE(ashe::VirtualKey::ToInteger("VK_SHIFT") == 0x10);
    REQUIRE(ashe::VirtualKey::ToInteger("VK_EXECUTE") == 0x2B);
    REQUIRE(ashe::VirtualKey::ToInteger("VK_RETURN") == 0x0D);

    REQUIRE(ashe::VirtualKey::ToString(0x0D) == "VK_RETURN");
    REQUIRE(ashe::VirtualKey::ToString(0x2B) == "VK_EXECUTE");
}
#endif