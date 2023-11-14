/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022~2023 winsoft666 <winsoft666@outlook.com>.
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

#ifndef ASHE_SHA1_HPP__
#define ASHE_SHA1_HPP__
#include "ashe/config.hpp"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <ashe/arch.hpp>
#include <ashe/file.hpp>

namespace ashe {
class ASHE_API SHA1 {
   public:
    enum { REPORT_HEX = 0,
           REPORT_DIGIT = 1 };

    SHA1();
    ~SHA1();

    void reset();

    // Use this function to hash in binary data and strings
    void update(const unsigned char* data, unsigned int len);

    void final();

    // Get the final hash as a pre-formatted string
    void reportHash(char* szReport, size_t bufSize, unsigned char uReportType = REPORT_HEX);

    // Get the raw message digest
    void getHash(unsigned char* uDest);

    static std::string GetFileSHA1(const fs::path& filePath);

    static std::string GetDataSHA1(const unsigned char* data, size_t dataSize);

   private:
    typedef union {
        unsigned char c[64];
        uint32_t l[16];
    } SHA1_WORKSPACE_BLOCK;

    void transform(uint32_t state[5], const unsigned char buffer[64]);

    uint32_t m_state[5];
    uint32_t m_count[2];
    unsigned char m_buffer[64];
    unsigned char m_digest[20];
};
}  // namespace ashe
#endif  // !ASHE_SHA1_HPP__