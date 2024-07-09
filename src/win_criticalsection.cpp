#include "ashe/config.h"
#include "ashe/win_criticalsection.h"
#ifdef ASHE_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif  // !_INC_WINDOWS

namespace ashe {

class WinCriticalSection::Private {
   public:
    CRITICAL_SECTION crit_;
};

WinCriticalSection::WinCriticalSection() noexcept :
    p_(new(std::nothrow) WinCriticalSection::Private()) {
    if (p_) {
        InitializeCriticalSection(&p_->crit_);
    }
}

WinCriticalSection::~WinCriticalSection() noexcept {
    if (p_) {
        DeleteCriticalSection(&p_->crit_);

        delete p_;
        p_ = nullptr;
    }
}

void WinCriticalSection::enter() const noexcept {
    if (p_) {
        EnterCriticalSection(&p_->crit_);
    }
}

void WinCriticalSection::leave() const noexcept {
    if (p_) {
        LeaveCriticalSection(&p_->crit_);
    }
}

bool WinCriticalSection::tryEnter() const noexcept {
    bool ret = false;
    if (p_) {
        ret = TryEnterCriticalSection(&p_->crit_) != FALSE;
    }
    return ret;
}
}  // namespace ashe
#endif