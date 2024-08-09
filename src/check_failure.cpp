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
    TraceW(L"Unexpected exception at %s: %s, %s\n", fileLine.c_str(), description.c_str(), strWhat.c_str());
    assert(false);
}

#ifdef ASHE_WIN
bool CheckFailure(HRESULT hr, const std::wstring& fileLine, const std::wstring& description) {
    if (FAILED(hr)) {
        TraceW(L"Failure at %s: %s, (0x%08X) %s\n", fileLine.c_str(), description.c_str(), hr, StrTrim(Win32ErrCodeToStr(hr)).c_str());
        assert(false);
        return true;
    }
    return false;
}
#endif  // ASHE_WIN

bool CheckFailure(bool result, const std::wstring& fileLine, const std::wstring& description) {
    if (!result) {
        TraceW(L"Failure at %s: %s\n", fileLine.c_str(), description.c_str());
        assert(false);
    }
    return !result;
}
}  // namespace ashe