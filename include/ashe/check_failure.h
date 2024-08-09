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

#ifndef ASHE_CHECK_FAILURE_H_
#define ASHE_CHECK_FAILURE_H_
#pragma once

#include "ashe/arch.h"
#include "ashe/config.h"
#ifdef ASHE_WIN
#include "ashe/windows_lite.h"
#endif  // ASHE_WIN
#include <string>

namespace ashe {
#define CHECK_FAILURE_STRINGIFY(arg) #arg

void ShowUnexpectedException(const std::wstring& fileLine, const std::wstring& description, const std::exception& e);

#ifdef ASHE_WIN
// Return true if failure occurred
bool CheckFailure(HRESULT hr, const std::wstring& fileLine, const std::wstring& description);

#define ASHE_CHECK_FAILURE_HRESULT_FILE_LINE(hr, desc, file, line) CheckFailure((hr), L"" CHECK_FAILURE_STRINGIFY(file) L"(" CHECK_FAILURE_STRINGIFY(line) L")", desc)

// Using this macro
#define ASHE_CHECK_FAILURE_HRESULT(hr, desc) ASHE_CHECK_FAILURE_HRESULT_FILE_LINE(hr, desc, __FILE__, __LINE__)

// Return true if failure occurred
bool CheckFailureLSTATUS(LSTATUS ls, const std::wstring& fileLine, const std::wstring& description);

#define ASHE_CHECK_FAILURE_STATUS_FILE_LINE(ls, desc, file, line) CheckFailureLSTATUS((ls), L"" CHECK_FAILURE_STRINGIFY(file) L"(" CHECK_FAILURE_STRINGIFY(line) L")", desc)

// Using this macro
#define ASHE_CHECK_FAILURE_STATUS(ls, desc) ASHE_CHECK_FAILURE_STATUS_FILE_LINE(ls, desc, __FILE__, __LINE__)

#endif  // ASHE_WIN

// Return true if failure occurred
bool CheckFailure(bool result, const std::wstring& fileLine, const std::wstring& description);

#define ASHE_UNEXPECTED_EXCEPTION_FILE_LINE(e, desc, file, line) ShowUnexpectedException(L"" CHECK_FAILURE_STRINGIFY(file) L"(" CHECK_FAILURE_STRINGIFY(line) L")", desc, e)

// Using this macro
#define ASHE_UNEXPECTED_EXCEPTION(e, desc) ASHE_UNEXPECTED_EXCEPTION_FILE_LINE((e), desc, __FILE__, __LINE__)

#define ASHE_CHECK_FAILURE_FILE_LINE(value, desc, file, line) CheckFailure((value), L"" CHECK_FAILURE_STRINGIFY(file) L"(" CHECK_FAILURE_STRINGIFY(line) L")", desc)

// Using this macro
#define ASHE_CHECK_FAILURE(value, desc) ASHE_CHECK_FAILURE_FILE_LINE(value, desc, __FILE__, __LINE__)

}  // namespace ashe

#endif  // !ASHE_CHECK_FAILURE_H_
