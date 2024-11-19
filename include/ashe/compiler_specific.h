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

#ifndef ASHE_COMPILER_SPECIFIC_H_
#define ASHE_COMPILER_SPECIFIC_H_
#pragma once

#ifndef ASHE_CPP_STANDARD_VER
#ifdef _MSC_VER
#define ASHE_CPP_STANDARD_VER _MSVC_LANG
#else
#define ASHE_CPP_STANDARD_VER __cplusplus
#endif
#endif

#if defined(_MSC_VER) && _MSC_VER >= 1500  // MSVC 2008
/// Indicates that the following function is deprecated.
#define ASHE_DEPRECATED(message) __declspec(deprecated(message))
#elif defined(__clang__) && defined(__has_feature)
#if __has_feature(attribute_deprecated_with_message)
#define ASHE_DEPRECATED(message) __attribute__((deprecated(message)))
#endif
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5))
#define ASHE_DEPRECATED(message) __attribute__((deprecated(message)))
#elif defined(__GNUC__) && (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define ASHE_DEPRECATED(message) __attribute__((__deprecated__))
#endif

#if !defined(ASHE_DEPRECATED)
#define ASHE_DEPRECATED(message)
#endif  // ! ASHE_DEPRECATED

#ifndef ASHE_NODISCARD
#if ASHE_CPP_STANDARD_VER >= 201703L
#define ASHE_NODISCARD [[nodiscard]]
#elif defined(_MSC_VER) && _MSC_VER >= 1700
#define ASHE_NODISCARD _Check_return_
#elif defined(__clang__)
#define ASHE_NODISCARD __attribute__((warn_unused_result))
#else
#define ASHE_NODISCARD
#endif
#endif

#endif  // !ASHE_COMPILER_SPECIFIC_H_
