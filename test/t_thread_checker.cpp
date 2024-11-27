#include "catch.hpp"
#include <thread>
#include "ashe/thread_checker.h"

using namespace ashe;

TEST_CASE("ThreadCheck-1", "") {
    THREAD_CHECKER(thread_checker1);
    THREAD_CHECKER(thread_checker2);
    DCHECK_CALLED_ON_VALID_THREAD(thread_checker1);
    DCHECK_CALLED_ON_VALID_THREAD(thread_checker1);
}

#if 0
TEST_CASE("ThreadCheck-2", "") {
    THREAD_CHECKER(thread_checker1);
    std::thread t = std::thread([&]() {
        bool res = DCHECK_CALLED_ON_VALID_THREAD(thread_checker1);
        REQUIRE(res);
    });
    t.join();
}
#endif