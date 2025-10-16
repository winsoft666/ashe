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

#ifndef ASHE_RSA_H__
#define ASHE_RSA_H__
#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace ashe {
// 非标准的RSA加密/解密实现，与OpenSSL不兼容
//
void RSAGenKeys(std::string& pub, std::string& priv);

bool RSAEncrypt(const std::vector<uint8_t>& plain,
                const std::string& pub,
                std::string& cipher);

bool RSAEncrypt(const std::string& plain,
                const std::string& pub,
                std::string& cipher);

bool RSADecrypt(const std::string& cipher,
                const std::string& priv,
                std::vector<uint8_t>& plain);

bool RSADecrypt(const std::string& cipher,
                const std::string& priv,
                std::string& plain);
}  // namespace ashe
#endif  // !ASHE_RSA_H__
