#include "ashe/config.hpp"
#include "ashe/trace.hpp"
#include "ashe/arch.hpp"
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
#include "ashe/string_helper.hpp"

namespace ashe {
void Trace::MsgW(const wchar_t* lpFormat, ...) noexcept {
    std::wstring output;
    va_list args;
    va_start(args, lpFormat);
    const bool ret = StringHelper::StringPrintfV(lpFormat, args, output);
    va_end(args);

    if (ret) {
#ifdef ASHE_WIN
        OutputDebugStringW(output.c_str());
#else
        printf("%ls", output.c_str());
#endif
    }
}

void Trace::MsgA(const char* lpFormat, ...) noexcept {
    std::string output;
    va_list args;
    va_start(args, lpFormat);
    const bool ret = StringHelper::StringPrintfV(lpFormat, args, output);
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