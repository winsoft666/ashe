#include "ashe/check_failure.h"
#include <assert.h>
#include "ashe/string_helper.h"
#include "ashe/string_encode.h"
#include "ashe/trace.h"

namespace ashe {
namespace internal {
void ShowUnexpectedException(const std::exception& e, const wchar_t* file, const wchar_t* func, int line, const wchar_t* description) {
    std::wstring strWhat;
#ifdef ASHE_WIN
    strWhat = a2w(e.what());
#else
    strWhat = u2w(e.what());
#endif
    if (!description || !description[0]) {
        TraceW(L"Unexpected exception at %s:%s(%d): %s\n", file, func, line, strWhat.c_str());
    }
    else {
        TraceW(L"Unexpected exception at %s:%s(%d): %s, %s\n", file, func, line, description, strWhat.c_str());
    }
    assert(false);
}

#ifdef ASHE_WIN
bool CheckFailureHRESULT(HRESULT hr, const wchar_t* file, const wchar_t* func, int line, const wchar_t* description) {
    if (FAILED(hr)) {
        if (!description || !description[0]) {
            TraceW(L"Failure at %s:%s(%d): (0x%08X) %s\n", file, func, line, hr, StrTrim(Win32ErrCodeToStr(hr)).c_str());
        }
        else {
            TraceW(L"Failure at %s:%s(%d): %s, (0x%08X) %s\n", file, func, line, description, hr, StrTrim(Win32ErrCodeToStr(hr)).c_str());
        }
        assert(false);
        return true;
    }
    return false;
}

bool CheckFailureLSTATUS(LSTATUS ls, const wchar_t* file, const wchar_t* func, int line, const wchar_t* description) {
    if (ls != ERROR_SUCCESS) {
        if (!description || !description[0]) {
            TraceW(L"Failure at %s:%s(%d): (%d) %s\n", file, func, line, ls, StrTrim(Win32ErrCodeToStr(ls)).c_str());
        }
        else {
            TraceW(L"Failure at %s:%s(%d): %s, (%d) %s\n", file, func, line, description, ls, StrTrim(Win32ErrCodeToStr(ls)).c_str());
        }
        assert(false);
        return true;
    }
    return false;
}

#endif  // ASHE_WIN

bool CheckFailureBool(bool result, const wchar_t* file, const wchar_t* func, int line, const wchar_t* description) {
    if (!result) {
        if (!description || !description[0]) {
            TraceW(L"Failure at %s:%s(%d)\n", file, func, line);
        }
        else {
            TraceW(L"Failure at %s:%s(%d): %s\n", file, func, line, description);
        }
        assert(false);
    }
    return !result;
}
}  // namespace internal
}  // namespace ashe