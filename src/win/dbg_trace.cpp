#include "ashe/config.h"
#include "ashe/win/dbg_trace.h"
#include <memory>
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#include <strsafe.h>
#include "ashe/string_helper.h"

namespace ashe {
namespace win {
void TraceW(const wchar_t* lpFormat, ...) {
    if (!lpFormat)
        return;
    std::wstring output;
    va_list args;
    va_start(args, lpFormat);
    const bool ret = StrFormat(lpFormat, args, output);
    va_end(args);

    if (ret) {
        OutputDebugStringW(output.c_str());
    }
}

void TraceA(const char* lpFormat, ...) {
    if (!lpFormat)
        return;
    std::string output;
    va_list args;
    va_start(args, lpFormat);
    const bool ret = StrFormat(lpFormat, args, output);
    va_end(args);

    if (ret) {
        OutputDebugStringA(output.c_str());
    }
}
}  // namespace win
}  // namespace ashe