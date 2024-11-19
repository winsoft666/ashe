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
#include "ashe/macros.h"
#include <string>

namespace ashe {
namespace internal {
#ifdef ASHE_WIN
// Return true if failure occurred
ASHE_API bool CheckFailureHRESULT(HRESULT hr,
                                  const wchar_t* file,
                                  const wchar_t* func,
                                  int line,
                                  const wchar_t* description);
ASHE_API bool CheckFailureLSTATUS(LSTATUS ls,
                                  const wchar_t* file,
                                  const wchar_t* func,
                                  int line,
                                  const wchar_t* description);
#endif  // ASHE_WIN
ASHE_API void ShowUnexpectedException(const std::exception& e,
                                      const wchar_t* file,
                                      const wchar_t* func,
                                      int line,
                                      const wchar_t* description);
ASHE_API bool CheckFailureBool(bool result,
                               const wchar_t* file,
                               const wchar_t* func,
                               int line,
                               const wchar_t* description);
}  // namespace internal

#ifdef ASHE_WIN
#define ASHE_CHECK_FAILURE_LSTATUS(ls, desc) \
    internal::CheckFailureLSTATUS(ls, _L(__FILE__), _L(CURRENT_FUNC_NAME), __LINE__, desc)

#define ASHE_CHECK_FAILURE_HRESULT(hr, desc) \
    internal::CheckFailureHRESULT(hr, _L(__FILE__), _L(CURRENT_FUNC_NAME), __LINE__, desc)
#endif  // ASHE_WIN

#define ASHE_UNEXPECTED_EXCEPTION(e, desc) \
    internal::ShowUnexpectedException((e), _L(__FILE__), _L(CURRENT_FUNC_NAME), __LINE__, desc)

#define ASHE_CHECK_FAILURE(value, desc) \
    internal::CheckFailureBool((value), _L(__FILE__), _L(CURRENT_FUNC_NAME), __LINE__, desc)

}  // namespace ashe

#endif  // !ASHE_CHECK_FAILURE_H_
