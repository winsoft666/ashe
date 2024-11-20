#include "ashe/config.h"
#include "ashe/win/hwnd_helper.h"

namespace ashe {
bool HwndHelper::BelongToSameProcess(HWND hwndA, HWND hwndB) {
    DWORD procAId = 0, procBId = 0;

    GetWindowThreadProcessId(hwndA, &procAId);
    GetWindowThreadProcessId(hwndB, &procBId);

    return (procAId == procBId);
}
}  // namespace ashe