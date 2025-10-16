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
// 启动当前进程的指定权限，如 SE_DEBUG_NAME、SE_SHUTDOWN_NAME 等
//
ASHE_API bool EnablePrivilege(LPCTSTR szPrivilege, bool fEnable);

// 判断进程是否以提升权限运行，可能是管理员、System 或 TrustedInstaller 权限
//
ASHE_API bool IsRunningAsElevation(HANDLE hProcess);
ASHE_API bool IsRunningAsElevation(DWORD dwPid);

// 设置窗口消息过滤器，允许指定的消息通过 UIPI
// 仅适用于 Windows Vista 及更高版本
// https://jiangxueqiao.com/post/2701401097.html
//
ASHE_API bool SetUIPIMsgFilter(HWND hWnd, unsigned int uMessageID, bool bAllow);

// 简化CreateProcess调用流程
//
ASHE_API bool CreateNewProcess(const std::wstring& path, const std::wstring& param, DWORD* dwPID, HANDLE* pProcess);

// 以管理员权限运行指定的程序
//
ASHE_API bool RunAsAdmin(const std::wstring& path, const std::wstring& param, int nShowCmd = SW_SHOWDEFAULT);

// 获取当前进程的可执行文件的完整路径
//
ASHE_API bool GetCurrentExePath(std::wstring& path);
ASHE_API bool GetCurrentExePath(std::string& path);
ASHE_API std::wstring GetCurrentExePathW();
ASHE_API std::string GetCurrentExePathA();

// 获取当前进程的可执行文件所在目录，以"\"结尾
//
ASHE_API bool GetCurrentExeDirectory(std::wstring& dir);
ASHE_API bool GetCurrentExeDirectory(std::string& dir);
ASHE_API std::wstring GetCurrentExeDirectoryW();
ASHE_API std::string GetCurrentExeDirectoryA();

ASHE_API bool Is32BitProcess(HANDLE process);
ASHE_API bool Is32BitProcess(unsigned long pid);
ASHE_API bool IsWow64Process(HANDLE process);
ASHE_API bool IsWow64Process(unsigned long pid);
ASHE_API bool Is64BitProcess(HANDLE process);
ASHE_API bool Is64BitProcess(unsigned long pid);

// 判断当前进程所加载的指定模块是否为64位
// 模块必须已经被当前进程加载
// pszModulePath可以是路径，也可以是模块文件名，如果为NULL，则表示当前进程
//
ASHE_API bool IsPeX64(LPCWSTR pszModulePath = NULL);

ASHE_API BOOL CALLBACK EnumResourceNameCallback(HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam);

ASHE_API bool GetExeOrDllManifest(const std::wstring& path, std::list<std::string>& manifests);

// 根据进程ID结束进程及其子进程
// 当所有进程都被成功结束时返回true，当有一个或多个进程结束失败时返回false
// 当结束某个进程失败时，不会中止操作，而是继续尝试结束其他进程
// 
// 如果需要查找进程是否存在，可以使用 ProcessFinder 类
//
ASHE_API bool KillProcessTree(unsigned long pid, unsigned int exitCode = 0);

// 根据进程ID结束进程
// 
// 如果需要查找进程是否存在，可以使用 ProcessFinder 类
//
ASHE_API bool KillProcess(unsigned long pid, unsigned int exitCode = 0);

// 根据进程ID获取进程可执行文件的完整路径
//
ASHE_API std::string GetProcessPathA(unsigned long pid);
ASHE_API std::wstring GetProcessPathW(unsigned long pid);

// 根据可执行文件名结束所有同名进程
// 当所有进程都被成功结束时返回true，当有一个或多个进程结束失败时返回false
// 当结束某个进程失败时，不会中止操作，而是继续尝试结束其他同名进程
// 
// 如果需要查找进程是否存在，可以使用 ProcessFinder 类
//
ASHE_API bool KillProcess(const std::wstring& exeName, unsigned int exitCode = 0);
ASHE_API bool KillProcess(const std::string& exeName, unsigned int exitCode = 0);
#else  // !ASHE_WIN
ASHE_API void KillProcessTree(pid_t id, bool force = false);
ASHE_API bool KillProcess(pid_t id, bool force = false);

ASHE_API std::string GetProcessPathA(pid_t id);
ASHE_API std::wstring GetProcessPathW(pid_t id);
#endif
}  // namespace ashe
#endif
