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
#ifndef ASHE_GUID_HPP__
#define ASHE_GUID_HPP__
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>
#include <cstdint>

namespace ashe {
// 生成一个新的UUID字符串，格式为 xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
// 返回的字符串使用小写字母
// 如果生成失败，返回空字符串
//
ASHE_API std::string CreateUuid();
}  // namespace ashe
#endif  // !ASHE_GUID_HPP__
