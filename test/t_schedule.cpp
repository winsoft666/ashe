#include "catch.hpp"
#include "ashe/arch.h"
#ifdef ASHE_WIN
#include "ashe/win/schedule_task.h"
#include "ashe/process_util.h"

using namespace ashe;
using namespace ashe::win;

TEST_CASE("ScheduleTest", "") {
    if (IsRunningAsElevation(GetCurrentProcess())) {
        const wchar_t pszTaskName[] = {L"test-task-name-1111"};
        ScheduleTask st;
        REQUIRE(st.createLoginTriggerTask(
            pszTaskName,
            L"c:\\test\\a.exe",
            L"test-params",
            L"test-desc",
            L"test-author"));
        std::wstring params;
        std::wstring programPath;
        REQUIRE(st.getParameters(pszTaskName, 1, params));
        REQUIRE(params == L"test-params");

        REQUIRE(st.getProgramPath(pszTaskName, 1, programPath));
        REQUIRE(programPath == L"c:\\test\\a.exe");

        REQUIRE(st.isExist(pszTaskName));
        REQUIRE(st.isTaskValid(pszTaskName));
        REQUIRE(st.isEnabled(pszTaskName));

        REQUIRE(st.setEnabled(pszTaskName, false));

        REQUIRE(st.isExist(pszTaskName));
        REQUIRE(!st.isTaskValid(pszTaskName));
        REQUIRE(!st.isEnabled(pszTaskName));

        REQUIRE(st.deleteTask(pszTaskName));
        REQUIRE(!st.isExist(pszTaskName));
        REQUIRE(!st.isTaskValid(pszTaskName));
        REQUIRE(!st.isEnabled(pszTaskName));
    }
}

#endif