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
#ifndef ASHE_PATH_HPP_
#define ASHE_PATH_HPP_
#pragma once
#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>

namespace ashe {
// 用于存储路径字符串，抹平不同平台的路径字符串的编码差异
//
// 在Windows平台，std::string或char*字符串视为ANSI编码，其他平台则视为UTF8编码
//
class ASHE_API Path {
   public:
    Path(const char* p);
    Path(const std::string& s);
    Path(std::string&& s);

    Path(const wchar_t* p);
    Path(const std::wstring& s);
    Path(std::wstring&& s);

    Path& operator=(const std::string& s);
    Path& operator=(std::string&& s) noexcept;

    Path& operator=(const std::wstring& s);
    Path& operator=(std::wstring&& s) noexcept;

    Path operator+(const Path& that);
    Path& operator+=(const Path& that);

    operator std::string() const;
    operator std::wstring() const;

    bool empty() const;

    // 返回当前平台的路径分隔符
    //
    static Path Separator();

   protected:
#ifdef ASHE_WIN
    std::wstring s_;
#else
    std::string s_;
#endif
};
}  // namespace ashe
#endif