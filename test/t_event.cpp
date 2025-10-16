#include "catch.hpp"
#include "ashe/event.h"
#include <thread>

TEST_CASE("Event1", "wait_after_set_1") {
    ashe::Event event;

    std::thread t1 = std::thread([&event]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        event.set();
    });

    std::thread t2 = std::thread([&event]() {
        bool waitRet = event.wait( 2000);
        REQUIRE(waitRet);
    });

    t1.join();
    t2.join();
}
