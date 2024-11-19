#include "ashe/check_failure.h"
#include <assert.h>
#include "ashe/string_helper.h"
#include "ashe/string_encode.h"
#include "ashe/trace.h"

namespace ashe {
namespace internal {
void ShowUnexpectedException(const std::exception& e, const char* file, const char* func, int line, const char* description) {
    if (!description || !description[0]) {
        TraceA("Unexpected exception at %s:%s(%d): %s\n",
               file, func, line, e.what());
    }
    else {
        TraceA("Unexpected exception at %s:%s(%d): %s, %s\n",
               file, func, line, description, e.what());
    }
    assert(false);
}

#ifdef ASHE_WIN
bool CheckFailureHRESULT(HRESULT hr, const char* file, const char* func, int line, const char* description) {
    if (FAILED(hr)) {
        if (!description || !description[0]) {
            TraceA("Failure at %s:%s(%d): (%#x) %s\n",
                   file, func, line, hr, w2a(StrTrim(Win32ErrCodeToStr(hr))).c_str());
        }
        else {
            TraceA("Failure at %s:%s(%d): %s, (%#x) %s\n",
                   file, func, line, description, hr, w2a(StrTrim(Win32ErrCodeToStr(hr))).c_str());
        }
        assert(false);
        return true;
    }
    return false;
}

bool CheckFailureLSTATUS(LSTATUS ls, const char* file, const char* func, int line, const char* description) {
    if (ls != ERROR_SUCCESS) {
        if (!description || !description[0]) {
            TraceA("Failure at %s:%s(%d): (%d) %s\n",
                   file, func, line, ls, w2a(StrTrim(Win32ErrCodeToStr(ls))).c_str());
        }
        else {
            TraceA("Failure at %s:%s(%d): %s, (%d) %s\n",
                   file, func, line, description, ls, w2a(StrTrim(Win32ErrCodeToStr(ls))).c_str());
        }
        assert(false);
        return true;
    }
    return false;
}

#endif  // ASHE_WIN

bool CheckFailureBool(bool result, const char* file, const char* func, int line, const char* description) {
    if (!result) {
        if (!description || !description[0]) {
            TraceA("Failure at %s:%s(%d)\n", file, func, line);
        }
        else {
            TraceA("Failure at %s:%s(%d): %s\n", file, func, line, description);
        }
        assert(false);
    }
    return !result;
}
}  // namespace internal
}  // namespace ashe