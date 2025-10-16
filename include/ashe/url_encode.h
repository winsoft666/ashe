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

#ifndef ASHE_URL_ENCODE_HPP__
#define ASHE_URL_ENCODE_HPP__
#pragma once

#include "ashe/config.h"
#include <string>
#include "ashe/hex_encode.h"

namespace ashe {
// URL编码
// 参数str：待编码字符串，需要以UTF-8编码
//
ASHE_API std::string UrlEncode(const std::string& str);

ASHE_API std::wstring UrlEncode(const std::wstring& str);

ASHE_API size_t UrlDecode(char* buffer, size_t buflen, const char* source, size_t srclen);

ASHE_API std::string UrlDecode(const std::string& source);

ASHE_API std::wstring UrlDecode(const std::wstring& source);

}  // namespace ashe
#endif  // !ASHE_URL_ENCODE_HPP__