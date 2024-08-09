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

#ifndef ASHE_HEX_ENCODE_HPP__
#define ASHE_HEX_ENCODE_HPP__
#include "ashe/config.h"
#include <cassert>
#include <string>

namespace ashe {
ASHE_API char HexEncode(unsigned char val);

ASHE_API bool HexDecode(char ch, unsigned char* val);

ASHE_API size_t HexEncodeWithDelimiter(char* buffer,
                                       size_t buflen,
                                       const char* csource,
                                       size_t srclen,
                                       char delimiter);

ASHE_API std::string HexEncode(const std::string& str);

ASHE_API std::string HexEncode(const char* source, size_t srclen);

ASHE_API std::string HexEncodeWithDelimiter(const char* source, size_t srclen, char delimiter);

ASHE_API size_t HexDecodeWithDelimiter(char* cbuffer,
                                       size_t buflen,
                                       const char* source,
                                       size_t srclen,
                                       char delimiter);

ASHE_API size_t HexDecode(char* buffer, size_t buflen, const std::string& source);

ASHE_API std::string HexDecode(const std::string& str);

ASHE_API size_t HexDecodeWithDelimiter(char* buffer,
                                       size_t buflen,
                                       const std::string& source,
                                       char delimiter);
}  // namespace ashe
#endif  // !ASHE_HEX_ENCODE_HPP__
