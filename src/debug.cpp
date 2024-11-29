#include "ashe/debug.h"
#ifdef ASHE_WIN
#include "ashe/win/windows_lite.h"
#endif

namespace ashe {
#ifdef ASHE_WIN
bool isDebuggerPresent() {
    return !!IsDebuggerPresent();
}
#endif

void debugBreak() {
#if defined(_MSC_VER)
    __debugbreak();
#else
    __asm__("int3");
#endif
}

}  // namespace ashe
