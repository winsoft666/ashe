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

// 存储并重置线程本地错误代码的值，并在析构函数中恢复它们
// 在 Windows 系统上错误代码通过 GetLastError/SetLastError 获取和设置
// 在 POSIX 系统上错误代码通过 errno 获取和设置
//
class ASHE_API ScopedClearLastError {
   public:
    ScopedClearLastError();
    ~ScopedClearLastError();

   private:
#ifdef ASHE_WIN
    const unsigned long lastError_;
#elif defined(ASHE_POSIX)
    const int lastError_;
#endif
    ASHE_DISALLOW_COPY_AND_ASSIGN(ScopedClearLastError);
};
}  // namespace ashe
#endif  // ASHE_SCOPED_CLEAR_LAST_ERROR_H_
