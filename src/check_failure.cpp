#include "ashe/check_failure.h"
#include <assert.h>
#include "ashe/string_helper.h"
#include "ashe/string_encode.h"
#include "ashe/trace.h"

namespace ashe {
void ShowUnexpectedException(const std::wstring& fileLine, const std::wstring& description, const std::exception& e) {
    std::wstring strWhat;
#ifdef ASHE_WIN
    strWhat = a2w(e.what());
#else
    strWhat = u2w(e.what());
#endif
    if (description.empty()) {
        TraceW(L"Unexpected exception at %s: %s\n", fileLine.c_str(), strWhat.c_str());
    }
    else {
        TraceW(L"Unexpected exception at %s: %s, %s\n", fileLine.c_str(), description.c_str(), strWhat.c_str());
    }
    assert(false);
}

#ifdef ASHE_WIN
bool CheckFailure(HRESULT hr, const std::wstring& fileLine, const std::wstring& description) {
    if (FAILED(hr)) {
        if (description.empty()) {
            TraceW(L"Failure at %s: (0x%08X) %s\n", fileLine.c_str(), hr, StrTrim(Win32ErrCodeToStr(hr)).c_str());
        }
        else {
            TraceW(L"Failure at %s: %s, (0x%08X) %s\n", fileLine.c_str(), description.c_str(), hr, StrTrim(Win32ErrCodeToStr(hr)).c_str());
        }
        assert(false);
        return true;
    }
    return false;
}

bool CheckFailureLSTATUS(LSTATUS ls, const std::wstring& fileLine, const std::wstring& description) {
    if (ls != ERROR_SUCCESS) {
        if (description.empty()) {
            TraceW(L"Failure at %s: (%d) %s\n", fileLine.c_str(), ls, StrTrim(Win32ErrCodeToStr(ls)).c_str());
        }
        else {
            TraceW(L"Failure at %s: %s, (%d) %s\n", fileLine.c_str(), description.c_str(), ls, StrTrim(Win32ErrCodeToStr(ls)).c_str());
        }
        assert(false);
        return true;
    }
    return false;
}

#endif  // ASHE_WIN

bool CheckFailure(bool result, const std::wstring& fileLine, const std::wstring& description) {
    if (!result) {
        if (description.empty()) {
            TraceW(L"Failure at %s\n", fileLine.c_str());
        }
        else {
            TraceW(L"Failure at %s: %s\n", fileLine.c_str(), description.c_str());
        }
        assert(false);
    }
    return !result;
}

}  // namespace ashe