#include "ashe/config.h"
#include "ashe/hwnd_helper.h"

#ifdef ASHE_WIN
namespace ashe {
bool HwndHelper::BelongToSameProcess(HWND hwndA, HWND hwndB) {
    DWORD procAId = 0, procBId = 0;

    GetWindowThreadProcessId(hwndA, &procAId);
    GetWindowThreadProcessId(hwndB, &procBId);

    return (procAId == procBId);
}
}  // namespace ashe
#endif