#include "ashe/scoped_clear_last_error.h"
#include <cerrno>

#if defined(ASHE_WIN)
#include "ashe/win/windows_lite.h"
#endif  // defined(ASHE_WIN)

namespace ashe {

ScopedClearLastErrorBase::ScopedClearLastErrorBase() :
    last_errno_(errno) {
    errno = 0;
}

ScopedClearLastErrorBase::~ScopedClearLastErrorBase() {
    errno = last_errno_;
}

#if defined(ASHE_WIN)
ScopedClearLastError::ScopedClearLastError() :
    last_error_(GetLastError()) {
    SetLastError(0);
}

ScopedClearLastError::~ScopedClearLastError() {
    SetLastError(last_error_);
}
#endif  // defined(ASHE_WIN)

}  // namespace ashe
