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

#ifndef ASHE_STRING_HELPER_HPP__
#define ASHE_STRING_HELPER_HPP__
#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>
#include <vector>

namespace ashe {
ASHE_API char ToLower(const char& in);
ASHE_API char ToUpper(const char& in);

ASHE_API wchar_t ToLower(const wchar_t& in);
ASHE_API wchar_t ToUpper(const wchar_t& in);

ASHE_API std::string ToLower(const std::string& s);
ASHE_API std::wstring ToLower(const std::wstring& s);

ASHE_API std::string ToUpper(const std::string& s);
ASHE_API std::wstring ToUpper(const std::wstring& s);

ASHE_API bool IsDigit(const std::string& s);
ASHE_API bool IsDigit(const std::wstring& s);

ASHE_API bool IsLetterOrDigit(const char& c);
ASHE_API bool IsLetterOrDigit(const wchar_t& c);

ASHE_API bool IsLetterOrDigit(const std::string& s);
ASHE_API bool IsLetterOrDigit(const std::wstring& s);

ASHE_API std::string Trim(const std::string& s, const std::string& whitespaces = " \t\f\v\n\r");
ASHE_API std::wstring Trim(const std::wstring& s, const std::wstring& whitespaces = L" \t\f\v\n\r");

ASHE_API std::string TrimLeft(const std::string& s, const std::string& whitespaces = " \t\f\v\n\r");
ASHE_API std::wstring TrimLeft(const std::wstring& s, const std::wstring& whitespaces = L" \t\f\v\n\r");

ASHE_API std::string TrimRight(const std::string& s, const std::string& whitespaces = " \t\f\v\n\r");
ASHE_API std::wstring TrimRight(const std::wstring& s, const std::wstring& whitespaces = L" \t\f\v\n\r");

ASHE_API bool IsStartsWith(const std::string& s, const std::string& prefix);
ASHE_API bool IsStartsWith(const std::wstring& s, const std::wstring& prefix);

ASHE_API bool IsEndsWith(const std::string& s, const std::string& suffix);
ASHE_API bool IsEndsWith(const std::wstring& s, const std::wstring& suffix);

ASHE_API bool IsContains(const std::string& str, const std::string& substring);
ASHE_API bool IsContains(const std::wstring& str, const std::wstring& substring);

ASHE_API size_t ContainTimes(const std::string& str, const std::string& substring);
ASHE_API size_t ContainTimes(const std::wstring& str, const std::wstring& substring);

ASHE_API std::string::size_type Find(const std::string& str, const std::string& substring, std::string::size_type offset = 0, bool caseInsensitive = false);
ASHE_API std::wstring::size_type Find(const std::wstring& str, const std::wstring& substring, std::wstring::size_type offset = 0, bool caseInsensitive = false);

ASHE_API std::string ReplaceFirst(const std::string& s, const std::string& from, const std::string& to);
ASHE_API std::wstring ReplaceFirst(const std::wstring& s, const std::wstring& from, const std::wstring& to);

ASHE_API std::string ReplaceLast(const std::string& s, const std::string& from, const std::string& to);
ASHE_API std::wstring ReplaceLast(const std::wstring& s, const std::wstring& from, const std::wstring& to);

ASHE_API std::string Replace(const std::string& s, const std::string& from, const std::string& to, std::string::size_type offset = 0, bool caseInsensitive = false);
ASHE_API std::wstring Replace(const std::wstring& s, const std::wstring& from, const std::wstring& to, std::wstring::size_type offset = 0, bool caseInsensitive = false);

ASHE_API std::vector<std::string> StrSplit(const std::string& src, const std::string& delimiter, bool includeEmptyStr = true);
ASHE_API std::vector<std::wstring> StrSplit(const std::wstring& src, const std::wstring& delimiter, bool includeEmptyStr = true);

ASHE_API std::string StrJoin(const std::vector<std::string>& src, const std::string& delimiter, bool includeEmptyStr = true);
ASHE_API std::wstring StrJoin(const std::vector<std::wstring>& src, const std::wstring& delimiter, bool includeEmptyStr = true);

ASHE_API bool IsEqual(const std::string& s1, const std::string& s2, bool ignoreCase = false);
ASHE_API bool IsEqual(const std::wstring& s1, const std::wstring& s2, bool ignoreCase = false);

// format a string
ASHE_API bool StringPrintfV(const char* format, va_list argList, std::string& output) noexcept;
ASHE_API bool StringPrintfV(const wchar_t* format, va_list argList, std::wstring& output) noexcept;

ASHE_API std::string StringPrintf(const char* format, ...) noexcept;
ASHE_API std::wstring StringPrintf(const wchar_t* format, ...) noexcept;

ASHE_API std::string StringPrintfV(const char* format, va_list argList) noexcept;
ASHE_API std::wstring StringPrintfV(const wchar_t* format, va_list argList) noexcept;

#ifdef ASHE_WIN
// such as @%SystemRoot%\\system32\\%SystemRoot%.dll,-10113
ASHE_API bool IsResourceString(const std::wstring& s);

// Notice: 32bit program can not load 64bit dll
ASHE_API bool LoadStringFromRes(const std::wstring& resStr, std::wstring& result);

// Get HRESULT/LSTATUS discription message.
ASHE_API std::wstring Win32ErrCodeToStr(long err);
#endif
}  // namespace ashe
#endif  // !ASHE_STRING_HELPER_HPP__
