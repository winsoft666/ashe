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

#ifndef ASHE_PROCESS_FINDER_HPP__
#define ASHE_PROCESS_FINDER_HPP__
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
#endif  // !_INC_WINDOWS
#include <TlHelp32.h>
#include <string>
#include "ashe/macros.h"

namespace ashe {
namespace win {
// Windows 进程查找器
//
class ASHE_API ProcessFinder {
   public:
    // dwFlags可以是下面值的任意组合：
    //    TH32CS_SNAPHEAPLIST
    //    TH32CS_SNAPPROCESS
    //    TH32CS_SNAPTHREAD
    //    TH32CS_SNAPMODULE
    //    TH32CS_SNAPMODULE32
    //    TH32CS_INHERIT
    //    TH32CS_SNAPALL = (TH32CS_SNAPHEAPLIST | TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD | TH32CS_SNAPMODULE)
    //
    // 如果要遍历系统中所有进程，dwProcessID必须为0
    //
    explicit ProcessFinder(DWORD dwFlags = TH32CS_SNAPALL, DWORD dwProcessID = 0);

    ProcessFinder(ProcessFinder&& that) noexcept;
    ProcessFinder& operator=(ProcessFinder&& that) noexcept;

    ~ProcessFinder();

    // 在ProcessFinder构造函数中会自动调用此函数，也可以在需要时手动调用此函数重新创建快照
    //
    bool createSnapShot(DWORD dwFlag, DWORD dwProcessID);

    // 返回进程列表中的第一个进程信息，会自动过滤掉进程ID为0的进程（System Idle Process）
    // processFirst与processNext配合使用，遍历系统中所有进程
    //
    // ppe参数必须指向一个有效的PROCESSENTRY32结构体，并且dwSize需要设置为sizeof(PROCESSENTRY32)
    // PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    // 
    bool processFirst(PPROCESSENTRY32 ppe) const;

    // 返回进程列表中的下一个进程信息
    // 
    // ppe参数必须指向一个有效的PROCESSENTRY32结构体，并且dwSize需要设置为sizeof(PROCESSENTRY32)
    // PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    //
    bool processNext(PPROCESSENTRY32 ppe) const;

    // 根据进程ID查找进程信息
    // 
    // ppe参数必须指向一个有效的PROCESSENTRY32结构体，并且dwSize需要设置为sizeof(PROCESSENTRY32)
    // PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    //
    bool processFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const;

    // 根据进程名查找进程信息
    // 
    // ppe参数必须指向一个有效的PROCESSENTRY32结构体，并且dwSize需要设置为sizeof(PROCESSENTRY32)
    // PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    // 
    // bExceptSelf 参数指定是否排除当前进程
    //
    bool processFind(PCTSTR pszExeName, PPROCESSENTRY32 ppe, BOOL bExceptSelf = false) const;

    // 返回模块列表中的第一个模块信息
    // 
    // pme参数必须指向一个有效的MODULEENTRY32结构体，并且dwSize需要设置为sizeof(MODULEENTRY32)
    // MODULEENTRY32 me32 = {sizeof(MODULEENTRY32)};
    //
    bool moduleFirst(PMODULEENTRY32 pme) const;

    // 返回模块列表中的下一个模块信息
    // 
    // pme参数必须指向一个有效的MODULEENTRY32结构体，并且dwSize需要设置为sizeof(MODULEENTRY32)
    // MODULEENTRY32 me32 = {sizeof(MODULEENTRY32)};
    //
    bool moduleNext(PMODULEENTRY32 pme) const;

    // 根据模块基址查找模块信息
    // 
    // pme参数必须指向一个有效的MODULEENTRY32结构体，并且dwSize需要设置为sizeof(MODULEENTRY32)
    // MODULEENTRY32 me32 = {sizeof(MODULEENTRY32)};
    //
    bool moduleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const;

    // 根据模块名或模块路径查找模块信息
    // 
    // pme参数必须指向一个有效的MODULEENTRY32结构体，并且dwSize需要设置为sizeof(MODULEENTRY32)
    // MODULEENTRY32 me32 = {sizeof(MODULEENTRY32)};
    //
    bool moduleFind(PTSTR pszModName, PMODULEENTRY32 pme) const;

    // 根据进程名或进程ID判断进程是否存在
    //
    static bool IsExist(const std::wstring& processName);
    static bool IsExist(const std::string& processName);
    static bool IsExist(const std::wstring& processName, DWORD* dwPID);
    static bool IsExist(const std::string& processName, DWORD* dwPID);
    static bool IsExist(DWORD dwPID);

   private:
    HANDLE hSnapShot_;

    friend void swap(ProcessFinder& first, ProcessFinder& second) noexcept;

    ASHE_DISALLOW_COPY_AND_ASSIGN(ProcessFinder);
};
}  // namespace win
}  // namespace ashe
#endif
