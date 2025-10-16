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

#ifndef ASHE_WIN_DBG_TRACE_H__
#define ASHE_WIN_DBG_TRACE_H__
#pragma once

#include "ashe/config.h"

namespace ashe {
namespace win {
// 打印日志到 DbgView 或 Visual Studio 的输出窗口
//
ASHE_API void TraceW(const wchar_t* lpFormat, ...);
ASHE_API void TraceA(const char* lpFormat, ...);
}  // namespace win
}  // namespace ashe

#endif  // !ASHE_WIN_DBG_TRACE_H__