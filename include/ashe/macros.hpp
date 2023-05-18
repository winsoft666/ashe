/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 winsoft666 <winsoft666@outlook.com>.
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

#ifndef ASHE_MACROS_HPP__
#define ASHE_MACROS_HPP__

#include <float.h>
#include <stdlib.h>

// Compare two float point value
#ifndef IS_NEARLY_EQUAL
#define IS_NEARLY_EQUAL(x, y) (fabs((x) - (y)) < FLT_EPSILON)
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p)       \
    do {                   \
        if ((p) != NULL) { \
            free(p);       \
            (p) = NULL;    \
        }                  \
    } while (false)
#endif

#ifndef SAFE_CLOSE_ON_NONULL
#define SAFE_CLOSE(p)         \
    do {                      \
        if ((p) != NULL) {    \
            CloseHandle((p)); \
            (p) = NULL;       \
        }                     \
    } while (false)
#endif

#ifndef SAFE_CLOSE_ON_VALID_HANDLE
#define SAFE_CLOSE_ON_VALID_HANDLE(p)      \
    do {                                   \
        if ((p) != INVALID_HANDLE_VALUE) { \
            CloseHandle((p));              \
            (p) = INVALID_HANDLE_VALUE;    \
        }                                  \
    } while (false)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) \
    do {                     \
        if ((p) != NULL) {   \
            delete[] (p);    \
            (p) = NULL;      \
        }                    \
    } while (false)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)     \
    do {                   \
        if ((p) != NULL) { \
            delete (p);    \
            (p) = NULL;    \
        }                  \
    } while (false)
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)     \
    do {                    \
        if ((p)) {          \
            (p)->Release(); \
            (p) = NULL;     \
        }                   \
    } while (false)
#endif

#ifndef ASHE_DISALLOW_MOVE
#define ASHE_DISALLOW_MOVE(TypeName)     \
    TypeName(const TypeName&&) = delete; \
    TypeName& operator=(const TypeName&&) = delete
#endif  // !ASHE_DISALLOW_MOVE

#ifndef ASHE_DISALLOW_COPY
#define ASHE_DISALLOW_COPY(TypeName)    \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete
#endif  // !ASHE_DISALLOW_COPY

#ifndef ASHE_DISALLOW_COPY_MOVE
#define ASHE_DISALLOW_COPY_MOVE(TypeName) \
    ASHE_DISALLOW_MOVE(TypeName);         \
    ASHE_DISALLOW_COPY(TypeName)
#endif  // !ASHE_DISALLOW_COPY_MOVE

#ifndef IS_FLAG_SET
#define IS_FLAG_SET(var, flag) (((var) & (flag)) == (flag))
#endif  // !IS_FLAG_SET

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

#define ASHE_ASSERT(condition) assert(condition)

// The call to assert() will show the failure message in debug builds.
// In release builds we abort, for a core-dump or debugger.
#define ASHE_FAIL_MESSAGE(message)          \
    {                                       \
        assert(false && message); \
        abort();                            \
    }

#define ASHE_ASSERT_MESSAGE(condition, message) \
    if (!(condition)) {                         \
        ASHE_FAIL_MESSAGE(message);             \
    }

#endif  // ! ASHE_MACROS_HPP__
