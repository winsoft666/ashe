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

#ifndef ASHE_WIN_DRIVER_INFO_HPP_
#define ASHE_WIN_DRIVER_INFO_HPP_
#pragma once
#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>
#include <assert.h>
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif  // !_INC_WINDOWS
#include <direct.h>

namespace ashe {
namespace win {
class ASHE_API DriverInfo {
   public:
    enum { DRIVE_HOST = 7 };

    static float GetFreeMB(int nDrive);

    static std::wstring GetVolume(int nDrive);

    static std::wstring GetRoot(int nDrive);

    static std::wstring GetFullName(int nDrive);

    static float GetTotalMB(int nDrive);

    static wchar_t GetLetter(int nDrive);

    static int GetType(int nDrive);

    static int GetPathType(const wchar_t* szPathName);

    static bool IsDriveAvailable(int nDrive);

    static bool IsUNCPath(const wchar_t* szPathName);

    static int GetDrive(const wchar_t* szPathName);

    static bool IsMappedPath(const wchar_t* szPathName);

    static int IsRemotePath(const wchar_t* szPathName, bool bAllowFileCheck = true);

    static bool IsFixedPath(const wchar_t* szPathName);

    static bool IsRemovablePath(const wchar_t* szPathName);

    // 检测驱动器是否为只读
    // 如果路径不存在，则返回-1
    // 1表示只读，0表示非只读
    //
    static int IsReadonlyPath(const wchar_t* szPathName);

    static unsigned long GetSerialNumber(int nDrive);
};
}  // namespace win
}  // namespace ashe
#endif  // !ASHE_WIN_DRIVER_INFO_HPP_