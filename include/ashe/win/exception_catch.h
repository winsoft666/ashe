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

#ifndef ASHE_WIN_MAIN_HPP_
#define ASHE_WIN_MAIN_HPP_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#include <tchar.h>
#include <DbgHelp.h>
#include <strsafe.h>

namespace ashe {
namespace win {
class ASHE_API WinExceptionCatchInternal {
   public:
    typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

    static TCHAR* lstrrchr(LPCTSTR string, int ch);

    static void DumpMiniDump(HANDLE hFile, PEXCEPTION_POINTERS excpInfo);

    static int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS pExceptPtrs, const TCHAR* szDumpNamePrefix);
};
}  // namespace win
}  // namespace ashe

#define WINMAIN_BEGIN(szDumpNamePrefix)                                                                                  \
    int __96A9695E_RUN_WINMAIN_FUNC(HINSTANCE hInstance, LPTSTR lpCmdLine);                                              \
    LONG WINAPI __96A9695E_UnhandledExceptionHandler(_EXCEPTION_POINTERS* pExceptionInfo) {                              \
        OutputDebugString(TEXT("Create a dump file sine an exception occurred in sub-thread.\n"));                       \
        int iRet = ashe::WinExceptionCatchInternal::RecordExceptionInfo(pExceptionInfo, szDumpNamePrefix);               \
        return iRet;                                                                                                     \
    }                                                                                                                    \
    int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine,                               \
                           int nCmdShow) {                                                                               \
        UNREFERENCED_PARAMETER(hPrevInstance);                                                                           \
        UNREFERENCED_PARAMETER(nCmdShow);                                                                                \
        ::SetUnhandledExceptionFilter(__96A9695E_UnhandledExceptionHandler);                                             \
        int ret = 0;                                                                                                     \
        __try {                                                                                                          \
            ret = __96A9695E_RUN_WINMAIN_FUNC(hInstance, lpCmdLine);                                                     \
        } __except (ashe::WinExceptionCatchInternal::RecordExceptionInfo(GetExceptionInformation(), szDumpNamePrefix)) { \
            OutputDebugString(TEXT("Create a dump file sine an exception occurred in main-thread.\n"));                  \
        }                                                                                                                \
        return ret;                                                                                                      \
    }                                                                                                                    \
    int __96A9695E_RUN_WINMAIN_FUNC(HINSTANCE hInstance, LPTSTR lpCmdLine) {
#define WINMAIN_END }

#define WMAIN_BEGIN(szDumpName)                                                                                    \
    int __96A9695E_RUN_MAIN_FUNC(int argc, wchar_t* argv[]);                                                       \
    LONG WINAPI __96A9695E_UnhandledExceptionHandler(_EXCEPTION_POINTERS* pExceptionInfo) {                        \
        OutputDebugString(TEXT("Create a dump file since an exception occurred in sub-thread.\n"));                \
        int iRet = ashe::WinExceptionCatchInternal::RecordExceptionInfo(pExceptionInfo, szDumpName);               \
        return iRet;                                                                                               \
    }                                                                                                              \
    int wmain(int argc, wchar_t* argv[]) {                                                                         \
        ::SetUnhandledExceptionFilter(__96A9695E_UnhandledExceptionHandler);                                       \
        int ret = 0;                                                                                               \
        __try {                                                                                                    \
            ret = __96A9695E_RUN_MAIN_FUNC(argc, argv);                                                            \
        } __except (ashe::WinExceptionCatchInternal::RecordExceptionInfo(GetExceptionInformation(), szDumpName)) { \
            OutputDebugString(                                                                                     \
                TEXT("Create a dump file since an exception occurred in "                                          \
                     "main-thread.\n"));                                                                           \
        }                                                                                                          \
        return ret;                                                                                                \
    }                                                                                                              \
    int __96A9695E_RUN_MAIN_FUNC(int argc, wchar_t* argv[]) {
#define MAIN_BEGIN(szDumpName)                                                                                     \
    int __96A9695E_RUN_MAIN_FUNC(int argc, char* argv[]);                                                          \
    LONG WINAPI __96A9695E_UnhandledExceptionHandler(_EXCEPTION_POINTERS* pExceptionInfo) {                        \
        OutputDebugString(TEXT("Create a dump file since an exception occurred in sub-thread.\n"));                \
        int iRet = ashe::WinExceptionCatchInternal::RecordExceptionInfo(pExceptionInfo, szDumpName);               \
        return iRet;                                                                                               \
    }                                                                                                              \
    int main(int argc, char* argv[]) {                                                                             \
        ::SetUnhandledExceptionFilter(__96A9695E_UnhandledExceptionHandler);                                       \
        int ret = 0;                                                                                               \
        __try {                                                                                                    \
            ret = __96A9695E_RUN_MAIN_FUNC(argc, argv);                                                            \
        } __except (ashe::WinExceptionCatchInternal::RecordExceptionInfo(GetExceptionInformation(), szDumpName)) { \
            OutputDebugString(                                                                                     \
                TEXT("Create a dump file since an exception occurred in "                                          \
                     "main-thread.\n"));                                                                           \
        }                                                                                                          \
        return ret;                                                                                                \
    }                                                                                                              \
    int __96A9695E_RUN_MAIN_FUNC(int argc, char* argv[]) {
#define WMAIN_END }
#define MAIN_END }

#endif  // !ASHE_WIN_MAIN_HPP_
