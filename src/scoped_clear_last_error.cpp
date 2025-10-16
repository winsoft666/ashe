#include "ashe/scoped_clear_last_error.h"
#include <cerrno>

#if defined(ASHE_WIN)
#include "ashe/win/windows_lite.h"
#endif  // defined(ASHE_WIN)

namespace ashe {
#if defined(ASHE_WIN)
ScopedClearLastError::ScopedClearLastError() :
    lastError_(GetLastError()) {
    SetLastError(0);
}

ScopedClearLastError::~ScopedClearLastError() {
    SetLastError(lastError_);
}
#elif defined(ASHE_POSIX)
ScopedClearLastErrorBase::ScopedClearLastErrorBase() :
    lastError_(errno) {
    errno = 0;
}

ScopedClearLastErrorBase::~ScopedClearLastErrorBase() {
    errno = lastError_;
}
#endif  // defined(ASHE_WIN)
}  // namespace ashe
