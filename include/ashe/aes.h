/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2020~2025 winsoft666 <winsoft666@outlook.com>.
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

#ifndef ASHE_AES_H__
#define ASHE_AES_H__
#pragma once

#include "ashe/config.h"
#include <vector>
#include <string>
#include <stdint.h>

namespace ashe {
// 如果填充模式为None，则明文的长度将自动扩展至 16 的整数倍，并以 0 进行填充
enum class AESPaddingMode {
    PKCS7 = 0,
    None = 1,
};

// 仅支持AES128-CBC模式
// key与iv的长度均为16
//
bool ASHE_API AES128CBCEncrypt(const std::vector<uint8_t>& key,
                               const std::vector<uint8_t>& iv,
                               AESPaddingMode paddingMode,
                               const std::vector<uint8_t>& plain,
                               std::vector<uint8_t>& cipher);

bool ASHE_API AES128CBCEncrypt(const std::string& key,
                               const std::string& iv,
                               AESPaddingMode paddingMode,
                               const std::string& plain,
                               std::string& cipher);

bool ASHE_API AES128CBCDecrypt(const std::vector<uint8_t>& key,
                               const std::vector<uint8_t>& iv,
                               AESPaddingMode paddingMode,
                               const std::vector<uint8_t>& cipher,
                               std::vector<uint8_t>& plain);

bool ASHE_API AES128CBCDecrypt(const std::string& key,
                               const std::string& iv,
                               AESPaddingMode paddingMode,
                               const std::string& cipher,
                               std::string& plain);
}  // namespace ashe
#endif  // !ASHE_AES_H__