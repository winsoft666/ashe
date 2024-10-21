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

#ifndef ASHE_OS_VER_HPP__
#define ASHE_OS_VER_HPP__
#include "ashe/config.h"
#include "ashe/arch.h"

#ifdef ASHE_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif  // !_INC_WINDOWS
#include <strsafe.h>
#elif defined(ASHE_MACOS)
#include <sys/sysctl.h>
#elif defined(ASHE_LINUX)
#include <sys/stat.h>
#include <sys/utsname.h>
#include <stdlib.h>
#include <cstring>
#include <fstream>
#include <regex>
#endif
#include <string>

#pragma warning(disable : 4996)

namespace ashe {
/*
Operating system                            dwMajorVersion     dwMinorVersion         dwBuild                      Other
Windows 11                                       10               0                   >=22000         Not applicable     
Windows 10 Technical Preview                     10               0                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server Technical Preview                 10               0                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 8.1                                      6                3                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2012 R2                           6                3                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 8                                        6                2                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2012                              6                2                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows 7                                        6                1                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2008 R2                           6                1                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows Server 2008                              6                0                                   OSVERSIONINFOEX.wProductType != VER_NT_WORKSTATION
Windows Vista                                    6                0                                   OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION
Windows Server 2003 R2                           5                2                                   GetSystemMetrics(SM_SERVERR2) != 0
Windows Home Server                              5                2                                   OSVERSIONINFOEX.wSuiteMask & VER_SUITE_WH_SERVER
Windows Server 2003                              5                2                                   GetSystemMetrics(SM_SERVERR2) == 0
Windows XP Professional x64 Edition              5                2                                   (OSVERSIONINFOEX.wProductType == VER_NT_WORKSTATION) && (SYSTEM_INFO.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
Windows XP                                       5                1                                   Not applicable
Windows 2000                                     5                0                                   Not applicable
*/

typedef struct _WinVerInfo {
    int major;
    int minor;
    int build;
    int productType;

    _WinVerInfo() {
        major = minor = build = productType = -1;
    }
} WinVerInfo;

#ifdef ASHE_WIN
ASHE_API WinVerInfo GetWinVer();

ASHE_API bool IsWindowsVistaOrHigher();

ASHE_API bool IsWindows7();

ASHE_API bool IsWindows8OrHigher();

ASHE_API bool IsWindows10OrHigher();

ASHE_API bool IsWindows11();

ASHE_API bool IsWin64();

#endif

ASHE_API std::string GetOSVersion();
}  // namespace ashe
#endif  // !ASHE_OS_VER_HPP__
