#include "catch.hpp"
#include "ashe/process.h"
#include "ashe/process_util.h"
#include <thread>

using namespace ashe;

// Test: create process, send data to process's input and get process's output.
//
TEST_CASE("ProcessTest") {
#ifdef ASHE_WIN
    Process proc(
        L"cmd.exe",
        Process::string_type(),
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        true);

    REQUIRE(proc.successed());

    int exitStatus = 0;
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(proc.write(std::string("ping 127.0.0.1\n")));
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    proc.killProcessTree();
    REQUIRE(proc.getExitStatus() > 0);
#else
    Process proc(
        "bash",
        Process::string_type(),
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        [](const char* bytes, size_t n) {
            const std::string str(bytes, n);
            printf("%s", str.c_str());
        },
        true);

    REQUIRE(proc.successed());

    int exitStatus = 0;
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(proc.tryGetExitStatus(exitStatus) == false);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    REQUIRE(proc.write(std::string("ping -c 6 127.0.0.1\n")));
    std::this_thread::sleep_for(std::chrono::seconds(3));
    proc.killProcessTree(true);
    REQUIRE(proc.getExitStatus() > 0);
#endif
}
