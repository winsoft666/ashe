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
#include <string>
#include <vector>
#if __cplusplus >= 201703L
#include <string_view>
#endif  // __cplusplus >= 201703L
#include <algorithm>
#include <stdexcept>

namespace ashe {
class ASHE_API Base64 {
   public:
    static std::string Encode(std::string const& s, bool url = false);

    static std::string EncodeWithPEM(std::string const& s);

    static std::string EncodeWithMIME(std::string const& s);

    static std::vector<uint8_t> Decode(std::string const& s, bool remove_linebreaks = false);

    static std::string Encode(unsigned char const* bytes_to_encode, size_t in_len, bool url = false);

#if __cplusplus >= 201703L
    static std::string Encode(std::string_view s, bool url = false);

    static std::string EncodeWithPEM(std::string_view s);

    static std::string EncodeWithMIME(std::string_view s);

    static std::vector<uint8_t> Decode(std::string_view s, bool remove_linebreaks = false);
#endif  // __cplusplus >= 201703L

   private:
    static const char* base64Chars(int index);

    static int getPosOfChar(const unsigned char chr);

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

    template <typename String>
    static bool _Decode(String encoded_string, bool remove_linebreaks, std::vector<uint8_t>& ret) {
        //
        // decode(…) is templated so that it can be used with String = const std::string&
        // or std::string_view (requires at least C++17)
        //
        if (encoded_string.empty())
            return false;

        if (remove_linebreaks) {
            encoded_string.erase(std::remove(encoded_string.begin(), encoded_string.end(), '\n'), encoded_string.end());
        }

        const size_t length_of_string = encoded_string.length();
        size_t pos = 0;

        //
        // The approximate length (bytes) of the decoded string might be one or
        // two bytes smaller, depending on the amount of trailing equal signs
        // in the encoded string. This approximation is needed to reserve
        // enough space in the string to be returned.
        //
        const size_t approx_length_of_decoded = length_of_string / 4 * 3;
        ret.reserve(approx_length_of_decoded);

        while (pos < length_of_string) {
            //
            // Iterate over encoded input string in chunks. The size of all
            // chunks except the last one is 4 bytes.
            //
            // The last chunk might be padded with equal signs or dots
            // in order to make it 4 bytes in size as well, but this
            // is not required as per RFC 2045.
            //
            // All chunks except the last one produce three output bytes.
            //
            // The last chunk produces at least one and up to three bytes.
            //

            int pos_of_char_1 = getPosOfChar(encoded_string[pos + 1]);
            if (pos_of_char_1 < 0)
                return false;

            //
            // Emit the first output byte that is produced in each chunk:
            //
            int pos_of_char_0 = getPosOfChar(encoded_string[pos + 0]);
            if (pos_of_char_0 < 0)
                return false;
            ret.push_back(static_cast<uint8_t>((pos_of_char_0 << 2) + ((pos_of_char_1 & 0x30) >> 4)));

            if ((pos + 2 < length_of_string) &&  // Check for data that is not padded with equal signs (which is allowed by RFC 2045)
                encoded_string[pos + 2] != '=' &&
                encoded_string[pos + 2] != '.'  // accept URL-safe base 64 strings, too, so check for '.' also.
            ) {
                //
                // Emit a chunk's second byte (which might not be produced in the last chunk).
                //
                int pos_of_char_2 = getPosOfChar(encoded_string[pos + 2]);
                if (pos_of_char_2 < 0)
                    return false;
                ret.push_back(static_cast<uint8_t>(((pos_of_char_1 & 0x0f) << 4) + ((pos_of_char_2 & 0x3c) >> 2)));

                if ((pos + 3 < length_of_string) &&
                    encoded_string[pos + 3] != '=' &&
                    encoded_string[pos + 3] != '.') {
                    //
                    // Emit a chunk's third byte (which might not be produced in the last chunk).
                    //
                    int pos_of_char_3 = getPosOfChar(encoded_string[pos + 3]);
                    if (pos_of_char_3 < 0)
                        return false;
                    ret.push_back(static_cast<uint8_t>(((pos_of_char_2 & 0x03) << 6) + pos_of_char_3));
                }
            }

            pos += 4;
        }

        return true;
    }
};
}  // namespace ashe
#endif  // !ASHE_BASE64_HPP__
