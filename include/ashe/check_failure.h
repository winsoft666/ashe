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
                                  const char* file,
                                  const char* func,
                                  int line,
                                  const char* description);
ASHE_API bool CheckFailureLSTATUS(LSTATUS ls,
                                  const char* file,
                                  const char* func,
                                  int line,
                                  const char* description);
#endif  // ASHE_WIN
ASHE_API void ShowUnexpectedException(const std::exception& e,
                                      const char* file,
                                      const char* func,
                                      int line,
                                      const char* description);
ASHE_API bool CheckFailureBool(bool result,
                               const char* file,
                               const char* func,
                               int line,
                               const char* description);
}  // namespace internal

#ifdef ASHE_WIN
#define ASHE_CHECK_FAILURE_LSTATUS(ls, desc) \
    internal::CheckFailureLSTATUS(ls, __FILE__, __func__, __LINE__, desc)

#define ASHE_CHECK_FAILURE_HRESULT(hr, desc) \
    internal::CheckFailureHRESULT(hr, __FILE__, __func__, __LINE__, desc)
#endif  // ASHE_WIN

#define ASHE_UNEXPECTED_EXCEPTION(e, desc) \
    internal::ShowUnexpectedException((e), __FILE__, __func__, __LINE__, desc)

#define ASHE_CHECK_FAILURE(value, desc) \
    internal::CheckFailureBool((value), __FILE__, __func__, __LINE__, desc)

}  // namespace ashe

#endif  // !ASHE_CHECK_FAILURE_H_
