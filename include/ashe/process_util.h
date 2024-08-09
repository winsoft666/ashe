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

#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Psapi.lib")
#endif

namespace ashe {
#ifdef ASHE_WIN
ASHE_API bool IsRunAsAdminPrivilege(HANDLE hProcess) noexcept;

ASHE_API bool IsRunAsAdminPrivilege(DWORD dwPid) noexcept;

ASHE_API bool SetUIPIMsgFilter(HWND hWnd, unsigned int uMessageID, bool bAllow) noexcept;

ASHE_API bool CreateNewProcess(const std::wstring& path, const std::wstring& param, DWORD* dwPID, HANDLE* pProcess);

ASHE_API bool RunAsAdmin(const std::wstring& path, const std::wstring& param, int nShowCmd = SW_SHOWDEFAULT);

ASHE_API bool Is32BitProcess(HANDLE process, bool& result) noexcept;

// Require user call free(buf).
ASHE_API bool GetCurrentExePath(wchar_t** buf);

ASHE_API bool GetCurrentExePath(char** buf);

ASHE_API std::wstring GetCurrentExePathW();
ASHE_API std::string GetCurrentExePathA();

ASHE_API std::wstring GetCurrentExeDirectoryW();
ASHE_API std::string GetCurrentExeDirectoryA();

// The module must have been loaded by the calling process.
ASHE_API bool IsPeX64(LPCWSTR pszModulePath = NULL);

ASHE_API BOOL CALLBACK EnumResourceNameCallback(HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam);

ASHE_API bool GetExeOrDllManifest(const std::wstring& path, std::list<std::string>& manifests);

#endif

#ifdef ASHE_WIN
ASHE_API void KillProcessTree(unsigned long pid) noexcept;

ASHE_API bool KillProcess(unsigned long pid) noexcept;

ASHE_API std::string GetProcessPathA(unsigned long pid) noexcept;
ASHE_API std::wstring GetProcessPathW(unsigned long pid) noexcept;

// Kill all process that executed file name is exeName.
// Return true when all process have been killed.
// Return false when have one or more process kill failed.
ASHE_API bool KillProcess(const std::wstring& exeName) noexcept;
ASHE_API bool KillProcess(const std::string& exeName) noexcept;

// Kill all process that EXE file in "dirPath" directory.
ASHE_API void RecursiveKillProcess(const std::wstring& dirPath, bool excludeSelf) noexcept;
ASHE_API void RecursiveKillProcess(const std::string& dirPath, bool excludeSelf) noexcept;
#else
ASHE_API void KillProcessTree(pid_t id, bool force = false) noexcept;
ASHE_API bool KillProcess(pid_t id, bool force = false) noexcept;

ASHE_API std::string GetProcessPathA(pid_t id) noexcept;
ASHE_API std::wstring GetProcessPathW(pid_t id) noexcept;
#endif
}  // namespace ashe
#endif
