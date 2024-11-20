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

#ifndef ASHE_BASE64_HPP__
#define ASHE_BASE64_HPP__
#pragma once

//
// Provided by https://github.com/ReneNyffenegger/cpp-base64, adapted by winsoft666.
//
#include "ashe/config.h"
#include "ashe/compiler_specific.h"
#include <string>
#include <vector>
#if ASHE_CPP_STANDARD_VER >= 201703L
#include <string_view>
#endif  // ASHE_CPP_STANDARD_VER >= 201703L
#include <algorithm>
#include <stdexcept>
#include <cstdint>

namespace ashe {
class ASHE_API Base64 {
   public:
    static std::string Encode(std::string const& s, bool url = false);

    static std::string EncodeWithPEM(std::string const& s);

    static std::string EncodeWithMIME(std::string const& s);

    static std::vector<uint8_t> Decode(std::string const& s, bool remove_linebreaks = false);

    static std::string Encode(unsigned char const* bytes_to_encode, size_t in_len, bool url = false);

#if ASHE_CPP_STANDARD_VER >= 201703L
    static std::string Encode(std::string_view s, bool url = false);

    static std::string EncodeWithPEM(std::string_view s);

    static std::string EncodeWithMIME(std::string_view s);

    static std::vector<uint8_t> Decode(std::string_view s, bool remove_linebreaks = false);
#endif  // ASHE_CPP_STANDARD_VER >= 201703L

   private:
    static const char* base64Chars(int index);

    static std::string insertLineBreaks(std::string str, size_t distance);

    template <typename String, unsigned int line_length>
    static std::string _EncodeWithLineBreaks(String s) {
        return insertLineBreaks(Encode(s, false), line_length);
    }

    template <typename String>
    static std::string _EncodeWithPEM(String s) {
        return _EncodeWithLineBreaks<String, 64>(s);
    }

    template <typename String>
    static std::string _EncodeWithMIME(String s) {
        return _EncodeWithLineBreaks<String, 76>(s);
    }

    template <typename String>
    static std::string _Encode(String s, bool url) {
        return Encode(reinterpret_cast<const unsigned char*>(s.data()), s.length(), url);
    }

};
}  // namespace ashe
#endif  // !ASHE_BASE64_HPP__
