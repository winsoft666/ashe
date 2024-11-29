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

#ifndef ASHE_SCOPED_CLEAR_LAST_ERROR_H_
#define ASHE_SCOPED_CLEAR_LAST_ERROR_H_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include "ashe/macros.h"

namespace ashe {

// ScopedClearLastError stores and resets the value of thread local error codes
// (errno, GetLastError()), and restores them in the destructor. This is useful
// to avoid side effects on these values in instrumentation functions that
// interact with the OS.

// Common implementation of ScopedClearLastError for all platforms. Use
// ScopedClearLastError instead.
class ASHE_API ScopedClearLastErrorBase {
   public:
    ScopedClearLastErrorBase();
    ~ScopedClearLastErrorBase();

   private:
    const int last_errno_;
    ASHE_DISALLOW_COPY_AND_ASSIGN(ScopedClearLastErrorBase);
};

#if defined(ASHE_WIN)

// Windows specific implementation of ScopedClearLastError.
class ASHE_API ScopedClearLastError final : public ScopedClearLastErrorBase {
   public:
    ScopedClearLastError();
    ~ScopedClearLastError();

   private:
    const unsigned long last_error_;
    ASHE_DISALLOW_COPY_AND_ASSIGN(ScopedClearLastError);
};

#elif defined(ASHE_POSIX)
using ScopedClearLastError = ScopedClearLastErrorBase;
#else
#error Platform support not implemented
#endif  // defined(ASHE_WIN)
}  // namespace ashe
#endif  // ASHE_SCOPED_CLEAR_LAST_ERROR_H_
