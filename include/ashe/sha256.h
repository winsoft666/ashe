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

#ifndef ASHE_SHA256_HPP__
#define ASHE_SHA256_HPP__
#include "ashe/config.h"
#include <string>
#include "ashe/path.h"

namespace ashe {
ASHE_API std::string GetFileSHA256(const Path& filePath);
ASHE_API std::string GetDataSHA256(const unsigned char* data, size_t dataSize);
}  // namespace ashe
#endif  // !ASHE_SHA256_HPP__