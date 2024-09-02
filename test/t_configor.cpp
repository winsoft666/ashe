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

TEST_CASE("Configor3", "Array") {
    std::string strCfg = "str = \"xxxx\";";
    std::istringstream ss(strCfg);

    ashe::Configor cfg(ss);

    cfg["arr"].set("str1", 0);
    cfg["arr"].set("str2", 1);
    cfg["arr"].set("str3", 4);
    cfg["arr"].set(123, 5);

    std::ostringstream outputStream;
    cfg.save(outputStream);

    printf("%s", outputStream.str().c_str());
}

TEST_CASE("Configor4", "Array") {
    std::string strCfg = "str = \"xxxx\";";
    std::istringstream ss(strCfg);

    ashe::Configor cfg(ss);

    cfg["arr"].set("str1", 0);
    cfg["arr"].set("str2", 1);
    cfg["arr"].set("str3", 4);
    cfg["arr"].set(123, 5);

    REQUIRE(cfg["arr"].elementExists("str2") == false);
    REQUIRE(cfg["arr"].valueSize() == 6);

    cfg["arr"].clearValues();

    REQUIRE(cfg["arr"].valueSize() == 0);

    cfg["arr"].set(123, 5);

    REQUIRE(cfg["arr"].valueSize() == 6);
    REQUIRE(cfg["arr"].getInt(0, -1) == -1);
    REQUIRE(cfg["arr"].getString(1, "xxx") == "");
    REQUIRE(cfg["arr"].getInt(5) == 123);

    std::ostringstream outputStream;
    cfg.save(outputStream);

    printf("%s", outputStream.str().c_str());
}
