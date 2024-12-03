#include "ashe/system_error.h"
#include "ashe/string_util.h"

#ifdef ASHE_WIN
#include "ashe/win/windows_lite.h"
#include "ashe/string_encode.h"
#elif defined(ASHE_LINUX) || defined(ASHE_MACOS)
#include <cstring>
#include <cerrno>
#endif

namespace ashe {
SystemError::SystemError(Code code) :
    code_(code) {
}

// static
SystemError SystemError::last() {
#if defined(ASHE_WIN)
    return SystemError(GetLastError());
#elif defined(ASHE_LINUX) || defined(ASHE_MACOS)
    return SystemError(errno);
#else
#error Platform support not implemented
#endif
}

SystemError::Code SystemError::code() const {
    return code_;
}

std::string SystemError::toString() {
    return toString(code_);
}

std::string SystemError::toString(Code code) {
#if defined(ASHE_WIN)
    LPWSTR buffer = NULL;
    DWORD dwRet = FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&buffer,
        NULL,
        NULL);

    if (dwRet > 0 && buffer) {
        std::wstring res = buffer;
        LocalFree(buffer);
        return w2u(res);
    }

    return StrFormat("Error (0x%x) while retrieving error. (0x%x)", GetLastError(), code);
#elif defined(ASHE_LINUX) || defined(ASHE_MACOS)
    return std::strerror(code);
#else
#error Platform support not implemented
#endif
}

}  // namespace ashe
