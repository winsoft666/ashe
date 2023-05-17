#include "catch.hpp"
#include "ashe/singleton_process.hpp"
#include <string>

TEST_CASE("SingletonProcess", "") {
    ashe::SingletonProcess::Instance()->markAndCheckStartup("test");
    ashe::SingletonProcess::Instance()->registerSecondaryCallback([](std::string secondaryCml) {
        printf("secondary startup: %s\n", secondaryCml.c_str());
    });


    getchar();
}