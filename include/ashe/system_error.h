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

#ifndef ASHE_SYSTEM_ERROR_H_
#define ASHE_SYSTEM_ERROR_H_

#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>

namespace ashe {
class ASHE_API SystemError {
   public:
#if defined(ASHE_WIN)
    using Code = unsigned long;
#elif defined(ASHE_LINUX) || defined(ASHE_MACOS)
    using Code = int;
#else
#error Platform support not implemented
#endif

    explicit SystemError(Code code);
    ~SystemError() = default;

    SystemError(const SystemError& other) = default;
    SystemError& operator=(const SystemError& other) = default;

    // Alias for GetLastError() on Windows and errno on POSIX. Avoids having to pull in Windows.h
    // just for GetLastError() and DWORD.
    static SystemError last();

    // Returns an error code.
    Code code() const;

    // Returns a string description of the error in UTF-8 encoding.
    std::string toString();

    static std::string toString(Code code);

   private:
    Code code_;
};
}  // namespace ashe

#endif  // ASHE_SYSTEM_ERROR_H_
