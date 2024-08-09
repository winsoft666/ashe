#include "catch.hpp"
#include "ashe/ipaddress.h"

// Test: ip address check.
//
TEST_CASE("IpAddressTest") {
    REQUIRE(ashe::IPAddress::IPIsLoopback(ashe::IPAddress("192.168.50.234")) == false);
    REQUIRE(ashe::IPAddress::IPIsLoopback(ashe::IPAddress("127.0.0.1")));
}