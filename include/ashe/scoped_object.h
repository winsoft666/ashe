/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2020~2024 winsoft666 <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef SCOPED_OBJECT_HPP__
#define SCOPED_OBJECT_HPP__
#include "ashe/config.h"
#include "ashe/arch.h"
#include <thread>
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
#include <combaseapi.h>
#else
#endif  // ASHE_WIN
#include "ashe/macros.h"
#include "check_failure.h"

namespace ashe {
#ifdef ASHE_WIN
// Simple HANDLE wrapper to close it automatically from the destructor.
//
class ScopedHandle {
   public:
    ASHE_DISALLOW_COPY_MOVE(ScopedHandle);
    explicit ScopedHandle(bool isNullInvalid = true) noexcept :
        kInvalidHandle_(isNullInvalid ? NULL : INVALID_HANDLE_VALUE) {
        handle_ = kInvalidHandle_;
    }

    ~ScopedHandle() noexcept { close(); }

    void close() {
        if (handle_ != kInvalidHandle_) {
            CloseHandle(handle_);
            handle_ = kInvalidHandle_;
        }
    }

    HANDLE detach() {
        HANDLE old_handle = handle_;
        handle_ = kInvalidHandle_;
        return old_handle;
    }

    operator HANDLE() const { return handle_; }
    HANDLE* operator&() { return &handle_; }
    HANDLE invalidHandle() const { return kInvalidHandle_; }

   private:
    const HANDLE kInvalidHandle_;
    HANDLE handle_;
};
#else
#endif

class ASHE_API ScopedFile {
   public:
    ASHE_DISALLOW_COPY_MOVE(ScopedFile);
    explicit ScopedFile(FILE* f) noexcept :
        f_(f) {
    }

    ~ScopedFile() noexcept {
        close();
    }

    operator FILE*() const { return f_; }
    FILE** operator&() { return &f_; }

    void close() {
        if (f_) {
            fclose(f_);
            f_ = nullptr;
        }
    }

    FILE* f_ = nullptr;
};

class ASHE_API ScopedComInitialize {
   public:
    ASHE_DISALLOW_COPY_MOVE(ScopedComInitialize);

    ScopedComInitialize() noexcept {
        ASHE_CHECK_FAILURE_HRESULT(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED), L"CoInitializeEx failed");
    }

    ~ScopedComInitialize() noexcept {
        ::CoUninitialize();
    }
};

class ASHE_API ScopedThread {
   public:
    ASHE_DISALLOW_COPY_MOVE(ScopedThread);

    explicit ScopedThread(std::thread& t) noexcept :
        t_(t) {
    }

    ~ScopedThread() noexcept {
        if (t_.joinable()) {
            t_.join();
        }
    }

   protected:
    std::thread& t_;
};
}  // namespace ashe
#endif  // !SCOPED_OBJECT_HPP__