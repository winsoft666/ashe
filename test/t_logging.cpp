#include "catch.hpp"
#include "ashe/logging.h"

using namespace ashe;

void DoSetLastError(int i) {
#ifdef ASHE_WIN
    SetLastError(i);
#else
    errno = i;
#endif
}

TEST_CASE("Logging-1", "") {
    for (int i = 0; i < 10; i++)
        LOG(LS_ERROR) << "ERROR MESSAGE(Logging-1) " << i;
}

TEST_CASE("Logging-2", "") {
    for (int i = 0; i < 10; i++) {
        DoSetLastError(i);
        PLOG(LS_ERROR) << "ERROR MESSAGE(Logging-2) " << i;
    }
}

#ifdef ASHE_WIN
TEST_CASE("Logging-3", "") {
    std::wstring w = L" hello";
    for (int i = 0; i < 10; i++) {
        DoSetLastError(i);
        PLOG(LS_ERROR) << "ERROR MESSAGE(Logging-3) " << i << w;
    }
}

TEST_CASE("Logging-4", "") {
    LoggingSettings settings;
    settings.minLogLevel = LOG_LS_INFO;

    initLogging(settings);
    std::wstring w = L" hello";
    for (int i = 0; i < 10; i++) {
        DoSetLastError(i);
        PLOG(LS_INFO) << "INFO MESSAGE(Logging-4) " << i << w;
    }
}

TEST_CASE("Logging-5", "") {
    LoggingSettings settings;
    settings.minLogLevel = LOG_LS_INFO;

    initLogging(settings);
    std::wstring w = L" hello";
    for (int i = 0; i < 10; i++) {
        DoSetLastError(i);
        PLOG(LS_INFO) << "INFO MESSAGE(Logging-5) " << i << w;
    }

    shutdownLogging();

    for (int i = 0; i < 10; i++) {
        DoSetLastError(i);
        PLOG(LS_INFO) << "INFO MESSAGE_2(Logging-5) " << i << w;
    }
}
#endif

TEST_CASE("Logging-6", "") {
    LoggingSettings settings;
    settings.minLogLevel = LOG_LS_INFO;

    initLogging(settings);

    std::string s = "123";

    LOG_IF(LS_INFO, s.length() != 3) << "INFO MESSAGE(Logging-6-1) " << "length =" << s.length();
    LOG_IF(LS_INFO, s.length() != 4) << "INFO MESSAGE(Logging-6-2) " << "length =" << s.length();
}

TEST_CASE("Logging-7", "") {
    LoggingSettings settings;
    settings.minLogLevel = LOG_LS_INFO;

    initLogging(settings);

    std::string s = "123";

    CHECK(s.length() == 3);
    CHECK(s.length() != 3);

    DoSetLastError(1);
    PCHECK(s.length() == 3);
    PCHECK(s.length() != 3);
}
