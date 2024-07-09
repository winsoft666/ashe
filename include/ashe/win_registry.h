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

#ifndef ASHE_WIN_REGISTRY_HPP_
#define ASHE_WIN_REGISTRY_HPP_
#pragma once

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
#endif
#include <vector>
#include <string>
#include "ashe/macros.h"

namespace ashe {
class ASHE_API WinRegistry {
   public:
    ASHE_DISALLOW_COPY_MOVE(WinRegistry);

    // hkeyRoot can be one of :
    // HKEY_CLASSES_ROOT
    // HKEY_CURRENT_CONFIG
    // HKEY_CURRENT_USER
    // HKEY_LOCAL_MACHINE
    // HKEY_USERS
    //
    WinRegistry(HKEY hkeyRoot, const std::wstring& subKey) noexcept;

    ~WinRegistry() noexcept;

    // samDesired:
    // https://docs.microsoft.com/zh-cn/windows/desktop/SysInfo/registry-key-security-and-access-rights
    // samDesired can be one of:
    // KEY_ALL_ACCESS,
    // KEY_QUERY_VALUE,
    // KEY_READ,
    // KEY_WRITE,
    // KEY_WOW64_32KEY,
    // KEY_WOW64_64KEY
    // and so on.
    //
    LSTATUS open(REGSAM samDesired, bool bCreate) noexcept;

    bool isOpen() const noexcept;

    HKEY getHandle() const noexcept;

    void close() noexcept;

    bool watchForChange(DWORD dwChangeFilter, bool bWatchSubtree) noexcept;

    LSTATUS waitForChange(DWORD dwChangeFilter, bool bWatchSubtree) noexcept;

    static LSTATUS DeleteKey(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValName, bool bPrefer64View) noexcept;

    static LSTATUS DeleteSubKeys(HKEY hKeyRoot, LPCWSTR lpSubKey, bool bPrefer64View) noexcept;

    LSTATUS getDWORDValue(LPCWSTR pszValueName, OUT DWORD& pdwDataOut) const noexcept;

    LSTATUS getBINARYValue(LPCWSTR pszValueName, LPBYTE pbDataOut, int cbDataOut) const noexcept;

    LSTATUS getSZValue(LPCWSTR pszValueName, OUT std::wstring& strValue) const noexcept;

    LSTATUS getExpandSZValue(LPCWSTR pszValueName,
                             bool bRetrieveExpandedString,
                             OUT std::wstring& strValue) const noexcept;

    LSTATUS getMultiSZValue(LPCWSTR pszValueName, OUT std::vector<std::wstring>& vStrValues) const noexcept;

    LSTATUS getValueBufferSize(LPCWSTR pszValueName, DWORD& dwSize) const noexcept;

    LSTATUS setDWORDValue(LPCWSTR pszValueName, DWORD dwData) noexcept;

    LSTATUS setBINARYValue(LPCWSTR pszValueName, const LPBYTE pbData, int cbData) noexcept;
    LSTATUS setSZValue(LPCWSTR pszValueName, const std::wstring& strData) noexcept;

    LSTATUS setExpandSZValue(LPCWSTR pszValueName, const std::wstring& strData) noexcept;

    LSTATUS setMultiSZValue(LPCWSTR pszValueName, const std::vector<std::wstring>& vStrValues) noexcept;

    LSTATUS getSubKeys(std::vector<std::wstring>& subKeys) noexcept;

   protected:
    void OnChange(HKEY hkey) noexcept;

   private:
    LSTATUS getValue(LPCWSTR pszValueName, DWORD dwTypeExpected, LPBYTE pbData, DWORD cbData) const noexcept;
    LSTATUS setValue(LPCWSTR pszValueName, DWORD dwValueType, const LPBYTE pbData, DWORD cbData) noexcept;
    LPWSTR createDoubleNulTermList(const std::vector<std::wstring>& vStrValues) const noexcept;

    static unsigned int _stdcall NotifyWaitThreadProc(LPVOID pvParam);
    static LSTATUS regDeleteKey32_64(HKEY hKey, LPCWSTR pszSubKey, bool bPrefer64View) noexcept;
    static LSTATUS regDeleteSubKeys(HKEY hKey, bool bPrefer64View) noexcept;
    static LSTATUS regDelSubKeysRecurse(HKEY hKeyRoot, LPWSTR lpSubKey, bool bPrefer64View) noexcept;

   private:
    HKEY hkeyRoot_;
    mutable HKEY hkey_;
    HANDLE hChangeEvent_;
    HANDLE hNotifyThread_;
    DWORD dwSamDesired_;
    DWORD dwChangeFilter_;
    std::wstring strSubKey_;
    bool bWatchSubtree_;
};
}  // namespace ashe
#endif  // !ASHE_WIN
#endif  // !ASHE_WIN_REGISTRY_HPP_
