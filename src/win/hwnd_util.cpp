#include "ashe/config.h"
#include "ashe/win/hwnd_util.h"

namespace ashe {
namespace win {
bool HwndBelongToSameProcess(HWND hwndA, HWND hwndB) {
    DWORD procAId = 0, procBId = 0;

    GetWindowThreadProcessId(hwndA, &procAId);
    GetWindowThreadProcessId(hwndB, &procBId);

    return (procAId == procBId);
}
}  // namespace win
}  // namespace ashe