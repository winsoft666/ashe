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
// 封装系统错误代码和描述
// 在 Windows 系统上，错误代码通过 GetLastError 获取和设置
// 在 POSIX 系统上，错误代码通过 errno 获取
//
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

    // 用于 Windows 系统的 GetLastError() 函数和 POSIX 系统的 errno 函数的别名
    // 避免了仅为了使用 GetLastError() 和 DWORD 而引入 Windows.h 头文件的情况
    //
    static SystemError last();

    Code code() const;

    // 返回错误代码对应的描述字符串（UTF8编码）
    //
    std::string toString();

    // 返回错误代码对应的描述字符串（UTF8编码）
    // 静态方法
    //
    static std::string toString(Code code);

   private:
    Code code_;
};
}  // namespace ashe

#endif  // ASHE_SYSTEM_ERROR_H_
