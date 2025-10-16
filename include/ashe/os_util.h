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

#ifndef ASHE_OS_UTIL_H_
#define ASHE_OS_UTIL_H_
#pragma once

#include "ashe/arch.h"
#include "ashe/config.h"

namespace ashe {
#ifdef ASHE_WIN
// Windows快速重启
ASHE_API bool OsFastReboot();

// Windows快速关机
ASHE_API bool OsFastShutdown();
#else
#endif
}  // namespace ashe

#endif  // !ASHE_OS_UTIL_H_
