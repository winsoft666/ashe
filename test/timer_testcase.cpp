#include "catch.hpp"
#include "ashe/all.hpp"

TEST_CASE("TimerTest1", "Test start and stop") {
    {
        ashe::Timer t;
        t.init();
        t.destory();
    }
}

TEST_CASE("TimerTest2", "Tests with two argument add") {
    ashe::Timer t;
    t.init();

    SECTION("Test uint64_t timeout argument") {
        int i = 0;
        t.add(100000, [&](std::size_t) { i = 42; });
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        REQUIRE(i == 42);
    }

    SECTION("Test duration timeout argument") {
        int i = 0;
        t.add(std::chrono::milliseconds(100), [&](std::size_t) { i = 43; });
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        REQUIRE(i == 43);
    }

    SECTION("Test time_point timeout argument") {
        int i = 0;
        t.add(std::chrono::steady_clock::now() + std::chrono::milliseconds(100), [&](std::size_t) { i = 44; });
        std::this_thread::sleep_for(std::chrono::milliseconds(120));
        REQUIRE(i == 44);
    }

    t.destory();
}

TEST_CASE("TimerTest3", "Tests with three argument add") {
    ashe::Timer t;
    t.init();

    SECTION("Test uint64_t timeout argument") {
        size_t count = 0;
        auto id = t.add(
            50000,  // 50ms
            [&](std::size_t) {
                ++count;
            },
            50000  // 50ms
        );
        std::this_thread::sleep_for(std::chrono::milliseconds(170));
        t.remove(id);
        REQUIRE(count == 3);
    }

    SECTION("Test duration timeout argument") {
        size_t count = 0;
        auto id =
            t.add(
                std::chrono::milliseconds(50),  // 50ms
                [&](std::size_t) { ++count; },
                std::chrono::microseconds(50000)  // 50ms
            );
        std::this_thread::sleep_for(std::chrono::milliseconds(235));
        t.remove(id);
        REQUIRE(count == 4);
    }

    t.destory();
}

TEST_CASE("TimerTest4", "Test delete timer in callback") {
    ashe::Timer t;

    t.init();

    SECTION("Delete one timer") {
        size_t count = 0;
        t.add(
            std::chrono::milliseconds(10), [&](std::size_t id) {
                ++count;
                t.remove(id);
            },
            std::chrono::milliseconds(10));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        REQUIRE(count == 1);
    }

    SECTION("Ensure that the correct timer is freed and reused") {
        auto id1 = t.add(std::chrono::milliseconds(1500), [](std::size_t) {});
        auto id2 = t.add(std::chrono::milliseconds(200), [&](std::size_t id) { t.remove(id); });
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        auto id3 = t.add(std::chrono::microseconds(1000), [](std::size_t) {});
        auto id4 = t.add(std::chrono::microseconds(2000), [](std::size_t) {});
        REQUIRE(id3 == id2);
        REQUIRE(id4 != id1);
        REQUIRE(id4 != id2);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    SECTION("Ensure that the correct timer is freed and reused - different ordering") {
        auto id1 = t.add(std::chrono::milliseconds(50), [&](std::size_t id) { t.remove(id); });
        auto id2 = t.add(std::chrono::milliseconds(150), [](std::size_t) {});
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        auto id3 = t.add(std::chrono::microseconds(150), [](std::size_t) {});
        auto id4 = t.add(std::chrono::microseconds(150), [](std::size_t) {});
        REQUIRE(id3 == id1);
        REQUIRE(id4 != id1);
        REQUIRE(id4 != id2);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    t.destory();
}

TEST_CASE("TimerTest5", "Test two identical timeouts") {
    int i = 0;
    int j = 0;
    ashe::Timer t;
    t.init();

    std::chrono::time_point<std::chrono::steady_clock> ts = std::chrono::steady_clock::now() + std::chrono::milliseconds(40);
    t.add(ts, [&](std::size_t) { i = 42; });
    t.add(ts, [&](std::size_t) { j = 43; });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    REQUIRE(i == 42);
    REQUIRE(j == 43);

    t.destory();
}

TEST_CASE("TimerTest6", "Test timeouts from the past.") {
    ashe::Timer t;
    t.init();

    SECTION("Test negative timeouts") {
        int i = 0;
        int j = 0;
        std::chrono::time_point<std::chrono::steady_clock> ts1 = std::chrono::steady_clock::now() - std::chrono::milliseconds(10);
        std::chrono::time_point<std::chrono::steady_clock> ts2 = std::chrono::steady_clock::now() - std::chrono::milliseconds(20);
        t.add(ts1, [&](std::size_t) { i = 42; });
        t.add(ts2, [&](std::size_t) { j = 43; });
        std::this_thread::sleep_for(std::chrono::microseconds(20));
        REQUIRE(i == 42);
        REQUIRE(j == 43);
    }

    SECTION("Test time overflow when blocking timer thread.") {
        int i = 0;
        std::chrono::time_point<std::chrono::steady_clock> ts1 = std::chrono::steady_clock::now() + std::chrono::milliseconds(10);
        std::chrono::time_point<std::chrono::steady_clock> ts2 = std::chrono::steady_clock::now() + std::chrono::milliseconds(20);
        t.add(ts1, [&](std::size_t) { std::this_thread::sleep_for(std::chrono::milliseconds(20)); });
        t.add(ts2, [&](std::size_t) { i = 42; });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        REQUIRE(i == 42);
    }

    t.destory();
}

TEST_CASE("TimerTest7", "Test order of multiple timeouts") {
    int i = 0;
    ashe::Timer t;
    t.init();
    t.add(10000, [&](std::size_t) { i = 42; });
    t.add(20000, [&](std::size_t) { i = 43; });
    t.add(30000, [&](std::size_t) { i = 44; });
    t.add(40000, [&](std::size_t) { i = 45; });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    REQUIRE(i == 45);
    t.destory();
}

TEST_CASE("TimerTest8", "Test with multiple timers") {
    int i = 0;
    ashe::Timer t1;
    t1.init();

    ashe::Timer t2;
    t2.init();

    SECTION("Update the same value at different times with different timers") {
        t1.add(std::chrono::milliseconds(200), [&](std::size_t) { i = 42; });
        t1.add(std::chrono::milliseconds(500), [&](std::size_t) { i = 43; });
        std::this_thread::sleep_for(std::chrono::milliseconds(300));
        REQUIRE(i == 42);
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        REQUIRE(i == 43);
    }

    SECTION("Remove one timer without affecting the other") {
        auto id1 = t1.add(std::chrono::milliseconds(30), [&](std::size_t) { i = 42; });
        t1.add(std::chrono::milliseconds(50), [&](std::size_t) { i = 43; });
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        t1.remove(id1);
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        REQUIRE(i == 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        REQUIRE(i == 43);
    }

    t1.destory();
    t2.destory();
}

TEST_CASE("TimerTest9", "Test remove timer id") {
    ashe::Timer t;
    t.init();

    SECTION("Remove out of range timer_id") {
        auto id = t.add(std::chrono::milliseconds(20), [](std::size_t) {});
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        auto res = t.remove(id + 1);
        REQUIRE(res == false);
    }

    t.destory();
}