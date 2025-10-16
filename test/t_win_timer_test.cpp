#include "catch.hpp"
#include "ashe/arch.h"
#ifdef ASHE_WIN
#include "ashe/win/timer.h"

using namespace ashe::win;

class Test {
   public:
    void TimerProc() {
        static int count = 0;
        printf("TimerProc: %d\n", ++count);
    }
};

TEST_CASE("WinTimer-1") {
    TTimer<Test> timer;
    Test t;
    timer.setTimedEvent(&t, &Test::TimerProc);
    timer.start(1000, false, false);
    Sleep(3500);
}

TEST_CASE("WinTimer-2") {
    Test t;

    Timer timer;
    timer.setTimedEvent([&t]() { t.TimerProc(); });
    timer.start(1000, false, false);
    Sleep(3500);
}
#endif