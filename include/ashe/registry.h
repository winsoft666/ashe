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
class ASHE_API Registry {
   public:
    ASHE_DISALLOW_COPY_MOVE(Registry);

    // hkeyRoot can be one of :
    // HKEY_CLASSES_ROOT
    // HKEY_CURRENT_CONFIG
    // HKEY_CURRENT_USER
    // HKEY_LOCAL_MACHINE
    // HKEY_USERS
    //
    Registry(HKEY hkeyRoot, const std::wstring& subKey) noexcept;

    ~Registry() noexcept;

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
    HRESULT open(REGSAM samDesired, bool bCreate) noexcept;

    bool isOpen() const noexcept;

    HKEY getHandle() const noexcept;

    void close() noexcept;

    bool watchForChange(DWORD dwChangeFilter, bool bWatchSubtree) noexcept;

    HRESULT waitForChange(DWORD dwChangeFilter, bool bWatchSubtree) noexcept;

    static bool DeleteKey(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValName, bool bPrefer64View) noexcept;

    static bool DeleteSubKeys(HKEY hKeyRoot, LPCTSTR lpSubKey, bool bPrefer64View) noexcept;

    HRESULT getDWORDValue(LPCWSTR pszValueName, OUT DWORD& pdwDataOut) const noexcept;

    HRESULT getBINARYValue(LPCWSTR pszValueName, LPBYTE pbDataOut, int cbDataOut) const noexcept;

    HRESULT getSZValue(LPCWSTR pszValueName, OUT std::wstring& strValue) const noexcept;

    HRESULT getExpandSZValue(LPCWSTR pszValueName,
                             bool bRetrieveExpandedString,
                             OUT std::wstring& strValue) const noexcept;

    HRESULT getMultiSZValue(LPCWSTR pszValueName, OUT std::vector<std::wstring>& vStrValues) const noexcept;

    HRESULT getValueBufferSize(LPCWSTR pszValueName, DWORD& dwSize) const noexcept;

    HRESULT setDWORDValue(LPCWSTR pszValueName, DWORD dwData) noexcept;

    HRESULT setBINARYValue(LPCWSTR pszValueName, const LPBYTE pbData, int cbData) noexcept;
    HRESULT setSZValue(LPCWSTR pszValueName, const std::wstring& strData) noexcept;

    HRESULT setExpandSZValue(LPCWSTR pszValueName, const std::wstring& strData) noexcept;

    HRESULT setMultiSZValue(LPCWSTR pszValueName, const std::vector<std::wstring>& vStrValues) noexcept;

    HRESULT getSubKeys(std::vector<std::wstring>& subKeys) noexcept;

   protected:
    void OnChange(HKEY hkey) noexcept;

   private:
    HRESULT getValue(LPCWSTR pszValueName, DWORD dwTypeExpected, LPBYTE pbData, DWORD cbData) const noexcept;
    HRESULT setValue(LPCWSTR pszValueName, DWORD dwValueType, const LPBYTE pbData, DWORD cbData) noexcept;
    LPWSTR createDoubleNulTermList(const std::vector<std::wstring>& vStrValues) const noexcept;

    static unsigned int _stdcall NotifyWaitThreadProc(LPVOID pvParam);
    static bool regDeleteKey32_64(HKEY hKey, LPCWSTR pszSubKey, bool bPrefer64View) noexcept;
    static bool regDeleteSubKeys(HKEY hKey, bool bPrefer64View) noexcept;
    static BOOL regDelSubKeysRecurse(HKEY hKeyRoot, LPTSTR lpSubKey, bool bPrefer64View) noexcept;

   private:
    HKEY m_hkeyRoot;
    mutable HKEY m_hkey;
    HANDLE m_hChangeEvent;
    HANDLE m_hNotifyThr;
    DWORD m_dwSamDesired;
    DWORD m_dwChangeFilter;
    std::wstring m_strSubKey;
    bool m_bWatchSubtree;
};
}  // namespace ashe
#endif  // !ASHE_WIN
#endif  // !ASHE_WIN_REGISTRY_HPP_
