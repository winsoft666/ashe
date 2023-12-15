#include "catch.hpp"
#include "ashe/all.hpp"

TEST_CASE("Event1", "wait_after_set_1") {
    ashe::Event event;

    std::thread t1 = std::thread([&event]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        event.set(true);
    });

    std::thread t2 = std::thread([&event]() {
        ashe::Event::DataType ret;
        bool waitRet = event.wait(ret, 2000);
        REQUIRE(waitRet);
        REQUIRE(ashe::get<bool>(ret));
    });

    t1.join();
    t2.join();
}
