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

#ifndef ASHE_WIN_PE_H_
#define ASHE_WIN_PE_H_
#pragma once

#include "ashe/arch.h"

#ifdef ASHE_WIN
#include "ashe/windows_lite.h"

#define PE_DOS_HEADER(base) ((PIMAGE_DOS_HEADER)base)
#define PE_NT_HEADER(base) ((PIMAGE_NT_HEADERS)((CHAR*)base + PE_DOS_HEADER(base)->e_lfanew))
#define PE_OPT_HEADER(base) ((PIMAGE_OPTIONAL_HEADER)(&PE_NT_HEADER(base)->OptionalHeader))
#define PE_OPT_HEADER32(base) ((PIMAGE_OPTIONAL_HEADER32)(&PE_NT_HEADER(base)->OptionalHeader))
#define PE_OPT_HEADER64(base) ((PIMAGE_OPTIONAL_HEADER64)(&PE_NT_HEADER(base)->OptionalHeader))
#define PE_X64(base) (PE_OPT_HEADER(base)->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)

#endif // ASHE_WIN

#endif  // !ASHE_WIN_PE_H_
