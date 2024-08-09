#include "ashe/config.h"
#include "ashe/trace.h"
#include "ashe/arch.h"
#include <memory>
#ifdef ASHE_WIN
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
#else
#include <string>
#include <stdarg.h>
#endif
#include "ashe/string_helper.h"

namespace ashe {
void TraceW(const wchar_t* lpFormat, ...) noexcept {
    if (!lpFormat)
        return;
    std::wstring output;
    va_list args;
    va_start(args, lpFormat);
    const bool ret = StrFormat(lpFormat, args, output);
    va_end(args);

    if (ret) {
#ifdef ASHE_WIN
        OutputDebugStringW(output.c_str());
#else
        printf("%ls", output.c_str());
#endif
    }
}

void TraceA(const char* lpFormat, ...) noexcept {
    if (!lpFormat)
        return;
    std::string output;
    va_list args;
    va_start(args, lpFormat);
    const bool ret = StrFormat(lpFormat, args, output);
    va_end(args);

    if (ret) {
#ifdef ASHE_WIN
        OutputDebugStringA(output.c_str());
#else
        printf("%s", output.c_str());
#endif
    }
}
}  // namespace ashe