#include "catch.hpp"
#include "ashe/configor.h"
#include <thread>
#include <sstream>

TEST_CASE("Configor1", "") {
    ashe::Configor cfg("test.cfg");
    std::thread t1 = std::thread([&cfg]() {
        for (int i = 0; i < 10; i++) {
            cfg["a"]["b"]["c"] = 10 + i;
        }
    });

    std::thread t2 = std::thread([&cfg]() {
        for (int i = 0; i < 10; i++) {
            cfg["a"]["b"]["c"] = 30 + i;
        }
    });

    t1.join();
    t2.join();
}


TEST_CASE("Configor2", "") {
    std::string strCfg = "str = \"xxxx\";";
    std::istringstream ss(strCfg);

    ashe::Configor cfg(ss);
    std::thread t1 = std::thread([&cfg]() {
        for (int i = 0; i < 10; i++) {
            cfg["a"]["b"]["c"] = 10 + i;
        }
    });

    std::thread t2 = std::thread([&cfg]() {
        for (int i = 0; i < 10; i++) {
            cfg["a"]["b"]["c"] = 30 + i;
        }
    });

    t1.join();
    t2.join();

    std::ostringstream outputStream;
    cfg.save(outputStream);

    printf("%s", outputStream.str().c_str());
}
