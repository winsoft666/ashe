#include "ashe/config.hpp"
#include "ashe/win_hwnd.hpp"

#ifdef ASHE_WIN
namespace ashe {
bool WinHwnd::BelongToSameProcess(HWND hwndA, HWND hwndB) {
    DWORD procAId = 0, procBId = 0;

    GetWindowThreadProcessId(hwndA, &procAId);
    GetWindowThreadProcessId(hwndB, &procBId);

    return (procAId == procBId);
}
}  // namespace ashe
#endif