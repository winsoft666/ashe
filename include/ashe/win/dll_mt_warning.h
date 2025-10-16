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

#ifndef ASHE_WIN_DLL_MT_WARNING_H_
#define ASHE_WIN_DLL_MT_WARNING_H_
#pragma once

// 当使用DLL方式编译时，如果DLL使用了MT(d)选项，则在传递std::string等参数时会导致CRT堆释放异常
//
#ifdef _MSC_VER
#ifdef ASHE_USE_STATIC_CRT
#pragma message("Warning: DLL compiled with MT(d) option will cause CRT heap release exception.")
#endif
#endif

#endif // !ASHE_WIN_DLL_MT_WARNING_H_
