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

#ifndef ASHE_STRING_ENCODE_HPP_
#define ASHE_STRING_ENCODE_HPP_
#include "ashe/config.h"
#include <string>
#include "ashe/arch.h"

namespace ashe {
#ifdef ASHE_WIN
ASHE_API std::string UnicodeToAnsi(const std::wstring& str, unsigned int code_page = 0) noexcept;
ASHE_API std::wstring AnsiToUnicode(const std::string& str, unsigned int code_page = 0) noexcept;
#endif

ASHE_API std::string UnicodeToUtf8(const std::wstring& str) noexcept;
ASHE_API std::string UnicodeToUtf8BOM(const std::wstring& str) noexcept;
ASHE_API std::wstring Utf8ToUnicode(const std::string& str) noexcept;

#ifdef ASHE_WIN
ASHE_API std::string AnsiToUtf8(const std::string& str, unsigned int code_page = 0) noexcept;
ASHE_API std::string AnsiToUtf8BOM(const std::string& str, unsigned int code_page = 0) noexcept;
ASHE_API std::string Utf8ToAnsi(const std::string& str, unsigned int code_page = 0) noexcept;
#endif

#ifdef ASHE_WIN
#if (defined UNICODE || defined _UNICODE)
#define TCHARToAnsi(str) ashe::UnicodeToAnsi((str), 0)
#define TCHARToUtf8(str) ashe::UnicodeToUtf8((str))
#define AnsiToTCHAR(str) ashe::AnsiToUnicode((str), 0)
#define Utf8ToTCHAR(str) ashe::Utf8ToUnicode((str))
#define TCHARToUnicode(str) ((std::wstring)(str))
#define UnicodeToTCHAR(str) ((std::wstring)(str))
#else
#define TCHARToAnsi(str) ((std::string)(str))
#define TCHARToUtf8 ashe::AnsiToUtf8((str), 0)
#define AnsiToTCHAR(str) ((std::string)(str))
#define Utf8ToTCHAR(str) ashe::Utf8ToAnsi((str), 0)
#define TCHARToUnicode(str) ashe::AnsiToUnicode((str), 0)
#define UnicodeToTCHAR(str) ashe::UnicodeToAnsi((str), 0)
#endif
#endif
}  // namespace ashe
#endif  // ASHE_STRING_ENCODE_HPP_
