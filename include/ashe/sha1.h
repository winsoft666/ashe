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

#ifndef ASHE_SHA1_HPP__
#define ASHE_SHA1_HPP__
#include "ashe/config.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "ashe/arch.h"
#include "ashe/path.h"

namespace ashe {
class ASHE_API SHA1 {
   public:
    SHA1();
    ~SHA1();

    void reset();

    // Use this function to hash in binary data and strings
    void update(const unsigned char* data, unsigned int len);

    void final();

    // Get the final hash as a pre-formatted string
    void reportHash(char* szReport, size_t bufSize);

    // Get the raw message digest
    void getHash(unsigned char* uDest);

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

ASHE_API std::string GetFileSHA1(const Path& filePath);

ASHE_API std::string GetDataSHA1(const unsigned char* data, size_t dataSize);

}  // namespace ashe
#endif  // !ASHE_SHA1_HPP__