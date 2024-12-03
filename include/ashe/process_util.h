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

#ifndef ASHE_PROCESS_UTIL_H__
#define ASHE_PROCESS_UTIL_H__
#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>
#include <list>

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
#endif

namespace ashe {
#ifdef ASHE_WIN
// Such as: SE_DEBUG_NAME
ASHE_API bool EnablePrivilege(LPCTSTR szPrivilege, bool fEnable);

ASHE_API bool IsRunAsAdminPrivilege(HANDLE hProcess);

ASHE_API bool IsRunAsAdminPrivilege(DWORD dwPid);

ASHE_API bool SetUIPIMsgFilter(HWND hWnd, unsigned int uMessageID, bool bAllow);

ASHE_API bool CreateNewProcess(const std::wstring& path, const std::wstring& param, DWORD* dwPID, HANDLE* pProcess);

ASHE_API bool RunAsAdmin(const std::wstring& path, const std::wstring& param, int nShowCmd = SW_SHOWDEFAULT);

ASHE_API bool Is32BitProcess(HANDLE process);

ASHE_API bool GetCurrentExePath(std::wstring& path);
ASHE_API bool GetCurrentExePath(std::string& path);
ASHE_API std::wstring GetCurrentExePathW();
ASHE_API std::string GetCurrentExePathA();

// End with '\'
ASHE_API bool GetCurrentExeDirectory(std::wstring& dir);
ASHE_API bool GetCurrentExeDirectory(std::string& dir);
ASHE_API std::wstring GetCurrentExeDirectoryW();
ASHE_API std::string GetCurrentExeDirectoryA();

ASHE_API bool IsWow64Process(HANDLE process);
ASHE_API bool IsWow64Process(unsigned long pid);

ASHE_API bool IsX64Process(unsigned long pid);

// The module must have been loaded by the calling process.
ASHE_API bool IsPeX64(LPCWSTR pszModulePath = NULL);

ASHE_API BOOL CALLBACK EnumResourceNameCallback(HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam);

ASHE_API bool GetExeOrDllManifest(const std::wstring& path, std::list<std::string>& manifests);

ASHE_API void KillProcessTree(unsigned long pid);

ASHE_API bool KillProcess(unsigned long pid);

ASHE_API std::string GetProcessPathA(unsigned long pid);
ASHE_API std::wstring GetProcessPathW(unsigned long pid);

// Kill all process that executed file name is exeName.
// Return true when all process have been killed.
// Return false when have one or more process kill failed.
ASHE_API bool KillProcess(const std::wstring& exeName);
ASHE_API bool KillProcess(const std::string& exeName);
#else  // !ASHE_WIN
ASHE_API void KillProcessTree(pid_t id, bool force = false);
ASHE_API bool KillProcess(pid_t id, bool force = false);

ASHE_API std::string GetProcessPathA(pid_t id);
ASHE_API std::wstring GetProcessPathW(pid_t id);
#endif
}  // namespace ashe
#endif
