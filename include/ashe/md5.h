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

#ifndef ASHE_MD5_HPP__
#define ASHE_MD5_HPP__
#include "ashe/config.h"
#include <string>
#include <memory.h>
#include "ashe/path.h"

namespace ashe {
// Support large memory or file.
//
ASHE_API std::string GetDataMD5(const unsigned char* buffer, size_t buffer_size);
ASHE_API std::string GetFileMD5(const Path& filePath);
}  // namespace ashe
#endif  // !ASHE_MD5_HPP__