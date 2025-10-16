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

#ifndef ASHE_MACROS_HPP__
#define ASHE_MACROS_HPP__

#include <float.h>
#include <stdlib.h>
#include <chrono>
#include <assert.h>

// 比较两个浮点数是否近似相等
//
#ifndef IS_NEARLY_EQUAL
#define IS_NEARLY_EQUAL(x, y) (fabs((x) - (y)) < FLT_EPSILON)
#endif

// 使用free安全释放指针并将其置空
//
#ifndef SAFE_FREE
#define SAFE_FREE(p)       \
    do {                   \
        if ((p) != NULL) { \
            free(p);       \
            (p) = NULL;    \
        }                  \
    } while (false)
#endif

// 使用CloseHandle安全关闭句柄并将其置NULL
//
#ifndef SAFE_CLOSE_ON_NONULL
#define SAFE_CLOSE(p)         \
    do {                      \
        if ((p) != NULL) {    \
            CloseHandle((p)); \
            (p) = NULL;       \
        }                     \
    } while (false)
#endif

// 使用CloseHandle安全关闭句柄并将其置INVALID_HANDLE_VALUE
//
#ifndef SAFE_CLOSE_ON_VALID_HANDLE
#define SAFE_CLOSE_ON_VALID_HANDLE(p)      \
    do {                                   \
        if ((p) != INVALID_HANDLE_VALUE) { \
            CloseHandle((p));              \
            (p) = INVALID_HANDLE_VALUE;    \
        }                                  \
    } while (false)
#endif

// 使用delete[]安全释放数组指针并将其置空
//
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) \
    do {                     \
        if ((p) != NULL) {   \
            delete[] (p);    \
            (p) = NULL;      \
        }                    \
    } while (false)
#endif

// 使用delete安全释放指针并将其置空
//
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)     \
    do {                   \
        if ((p) != NULL) { \
            delete (p);    \
            (p) = NULL;    \
        }                  \
    } while (false)
#endif

// 使用COM对象的Release方法安全释放指针并将其置空
//
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)     \
    do {                    \
        if ((p)) {          \
            (p)->Release(); \
            (p) = NULL;     \
        }                   \
    } while (false)
#endif

// 禁止移动构造和移动赋值
//
#ifndef ASHE_DISALLOW_MOVE
#define ASHE_DISALLOW_MOVE(TypeName)     \
    TypeName(const TypeName&&) = delete; \
    TypeName& operator=(const TypeName&&) = delete
#endif  // !ASHE_DISALLOW_MOVE

// 禁止拷贝构造
//
#ifndef ASHE_DISALLOW_COPY
#define ASHE_DISALLOW_COPY(TypeName) \
    TypeName(const TypeName&) = delete;
#endif  // !ASHE_DISALLOW_COPY

// 禁止赋值操作
//
#ifndef ASHE_DISALLOW_ASSIGN
#define ASHE_DISALLOW_ASSIGN(TypeName) \
    TypeName& operator=(const TypeName&) = delete
#endif  // !ASHE_DISALLOW_ASSIGN

// 禁止拷贝构造和赋值操作
//
#ifndef ASHE_DISALLOW_COPY_AND_ASSIGN
#define ASHE_DISALLOW_COPY_AND_ASSIGN(TypeName)  \
    TypeName(const TypeName&) = delete; \
    TypeName& operator=(const TypeName&) = delete
#endif  // !ASHE_DISALLOW_COPY_AND_ASSIGN

// 禁止拷贝构造/赋值和移动构造/移动赋值
//
#ifndef ASHE_DISALLOW_COPY_AND_MOVE
#define ASHE_DISALLOW_COPY_AND_MOVE(TypeName) \
    ASHE_DISALLOW_MOVE(TypeName);         \
    ASHE_DISALLOW_COPY(TypeName)
#endif  // !ASHE_DISALLOW_COPY_AND_MOVE

// 判断变量var中是否包含flag标志
//
#ifndef IS_FLAG_SET
#define IS_FLAG_SET(var, flag) (((var) & (flag)) == (flag))
#endif  // !IS_FLAG_SET

// 判断 std::future 线程是否正在运行
//
#ifndef STD_ASYNC_IS_RUNNING
#define STD_ASYNC_IS_RUNNING(x) ((x).valid() && (x).wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
#endif

// 标记未使用的参数，避免编译器警告
//
#ifndef ASHE_UNUSED
#define ASHE_UNUSED(x) (void)(x)
#endif

// 将字符串字面值转换为宽字符串字面值
//
#ifndef _L
#define _L(x) __L(x)
#define __L(x) L##x
#endif

#endif  // ! ASHE_MACROS_HPP__
