#include "ashe/config.hpp"
#include "ashe/win_registry.hpp"
#ifdef ASHE_WIN
#include <process.h>
#include <strsafe.h>
#include "ashe/os_ver.hpp"

namespace ashe {
WinRegistry::WinRegistry(HKEY hkeyRoot, const std::wstring& subKey) noexcept :
    m_hkeyRoot(hkeyRoot),
    m_hkey(NULL),
    m_hChangeEvent(NULL),
    m_hNotifyThr(NULL),
    m_bWatchSubtree(false),
    m_dwChangeFilter(0),
    m_dwSamDesired(0),
    m_strSubKey(subKey) {}

WinRegistry::~WinRegistry() noexcept {
    close();

    if (m_hChangeEvent)
        CloseHandle(m_hChangeEvent);
}

HRESULT WinRegistry::open(REGSAM samDesired, bool bCreate) noexcept {
    LSTATUS dwResult;
    close();

    m_dwSamDesired = samDesired;
    if (bCreate) {
        DWORD dwDisposition;
        dwResult = RegCreateKeyExW(m_hkeyRoot, m_strSubKey.c_str(), 0, NULL, 0, samDesired, NULL,
                                   &m_hkey, &dwDisposition);
    }
    else {
        dwResult = RegOpenKeyExW(m_hkeyRoot, m_strSubKey.c_str(), 0, samDesired, &m_hkey);
    }

    return HRESULT_FROM_WIN32(dwResult);
}

bool WinRegistry::isOpen() const noexcept {
    return NULL != m_hkey;
}

HKEY WinRegistry::getHandle() const noexcept {
    return m_hkey;
}

void WinRegistry::close() noexcept {
    if (NULL != m_hkey) {
        HKEY hkeyTemp = m_hkey;
        m_hkey = NULL;
        RegCloseKey(hkeyTemp);
    }

    if (m_hNotifyThr) {
        WaitForSingleObject(m_hNotifyThr, INFINITE);
    }

    if (m_hNotifyThr) {
        CloseHandle(m_hNotifyThr);
        m_hNotifyThr = NULL;
    }

    m_dwSamDesired = 0;
}

bool WinRegistry::watchForChange(DWORD dwChangeFilter, bool bWatchSubtree) noexcept {
    if (NULL != m_hChangeEvent || NULL == m_hkey)
        return false;

    m_hChangeEvent = CreateEventW(NULL, FALSE, FALSE, NULL);

    if (NULL == m_hChangeEvent) {
        return false;
    }

    m_dwChangeFilter = dwChangeFilter;
    m_bWatchSubtree = bWatchSubtree;

    unsigned int uThreadId = 0;
    m_hNotifyThr = (HANDLE)_beginthreadex(NULL, 0, NotifyWaitThreadProc, this, 0, &uThreadId);

    return !!m_hNotifyThr;
}

HRESULT WinRegistry::waitForChange(DWORD dwChangeFilter, bool bWatchSubtree) noexcept {
    LSTATUS ls = RegNotifyChangeKeyValue(m_hkey, bWatchSubtree, dwChangeFilter, NULL, FALSE);
    return HRESULT_FROM_WIN32(ls);
}

bool WinRegistry::DeleteKey(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValName, bool bPrefer64View) noexcept {
    HKEY hSubKey = NULL;

    if (pszSubKey) {
        REGSAM rsam = KEY_READ | KEY_WRITE;

        if (bPrefer64View)
            rsam |= KEY_WOW64_64KEY;

        RegOpenKeyExW(hKey, pszSubKey, 0, rsam, &hSubKey);
    }
    else {
        hSubKey = hKey;
    }

    if (hSubKey) {
        if (pszValName) {
            if (RegDeleteValueW(hSubKey, pszValName) == ERROR_SUCCESS) {
                if (hSubKey != hKey) {
                    if (hSubKey)
                        RegCloseKey(hSubKey);
                }

                return true;
            }
        }
        else {
            if (regDeleteSubKeys(hSubKey, bPrefer64View)) {
                if (hSubKey != hKey) {
                    if (hSubKey)
                        RegCloseKey(hSubKey);
                }

                return regDeleteKey32_64(hKey, pszSubKey, bPrefer64View);
            }
        }

        if (hSubKey != hKey) {
            if (hSubKey)
                RegCloseKey(hSubKey);
        }
    }

    return false;
}

bool WinRegistry::DeleteSubKeys(HKEY hKeyRoot, LPCTSTR lpSubKey, bool bPrefer64View) noexcept {
    TCHAR szDelKey[MAX_PATH * 2];

    StringCchCopy(szDelKey, MAX_PATH * 2, lpSubKey);
    return regDelSubKeysRecurse(hKeyRoot, szDelKey, bPrefer64View) == TRUE;
}

HRESULT WinRegistry::getDWORDValue(LPCWSTR pszValueName, OUT DWORD& pdwDataOut) const noexcept {
    return getValue(pszValueName, REG_DWORD, (LPBYTE)(&pdwDataOut), sizeof(DWORD));
}

HRESULT WinRegistry::getBINARYValue(LPCWSTR pszValueName, LPBYTE pbDataOut, int cbDataOut) const noexcept {
    return getValue(pszValueName, REG_BINARY, pbDataOut, cbDataOut);
}

HRESULT WinRegistry::getSZValue(LPCWSTR pszValueName, OUT std::wstring& strValue) const noexcept {
    DWORD cb = 0;
    HRESULT hr = getValueBufferSize(pszValueName, cb);
    if (FAILED(hr)) {
        return hr;
    }

    if (cb == 0) {
        strValue = L"";
        return S_OK;
    }

    WCHAR* pTemp = (WCHAR*)malloc(cb + sizeof(WCHAR));
    if (!pTemp)
        return E_OUTOFMEMORY;

    memset(pTemp, 0, cb + sizeof(WCHAR));

    hr = getValue(pszValueName, REG_SZ, (LPBYTE)pTemp, cb);
    if (SUCCEEDED(hr))
        strValue = pTemp;

    SAFE_FREE(pTemp);

    return hr;
}

HRESULT WinRegistry::getExpandSZValue(LPCWSTR pszValueName,
                                      bool bRetrieveExpandedString,
                                      OUT std::wstring& strValue) const noexcept {
    WCHAR szBuf[MAX_PATH] = {0};
    DWORD dwSize = MAX_PATH * sizeof(WCHAR);
    DWORD dwFlags = RRF_RT_ANY | RRF_ZEROONFAILURE;
    if (!bRetrieveExpandedString) {
        dwFlags |= RRF_NOEXPAND;
        dwFlags |= RRF_RT_REG_EXPAND_SZ;
    }
    else {
        dwFlags |= RRF_RT_REG_SZ;
    }

    if (m_dwSamDesired & KEY_WOW64_64KEY)
        dwFlags |= RRF_SUBKEY_WOW6464KEY;
    else if (m_dwSamDesired & KEY_WOW64_32KEY)
        dwFlags |= RRF_SUBKEY_WOW6432KEY;
    else
        dwFlags |= RRF_SUBKEY_WOW6432KEY;

    LSTATUS status =
        RegGetValueW(m_hkeyRoot, m_strSubKey.c_str(), pszValueName, dwFlags, NULL, szBuf, &dwSize);
    if (status == ERROR_MORE_DATA) {
        WCHAR* pBuf = new (std::nothrow) WCHAR[dwSize / sizeof(WCHAR)];
        if (pBuf) {
            memset(pBuf, 0, dwSize);
            status =
                RegGetValueW(m_hkeyRoot, m_strSubKey.c_str(), pszValueName, dwFlags, NULL, pBuf, &dwSize);

            strValue = pBuf;
            SAFE_DELETE_ARRAY(pBuf);
        }
        else {
            status = ERROR_OUTOFMEMORY;
        }
    }
    else if (status == ERROR_SUCCESS) {
        strValue = szBuf;
    }

    return HRESULT_FROM_WIN32(status);
}

HRESULT WinRegistry::getMultiSZValue(LPCWSTR pszValueName, OUT std::vector<std::wstring>& vStrValues) const noexcept {
    DWORD cb = 0;
    HRESULT hr = getValueBufferSize(pszValueName, cb);

    if (FAILED(hr)) {
        return hr;
    }

    if (cb == 0) {
        vStrValues.clear();
        return S_OK;
    }

    WCHAR* pTemp = (WCHAR*)malloc(cb + sizeof(WCHAR));
    if (!pTemp)
        return E_OUTOFMEMORY;

    memset(pTemp, 0, cb + sizeof(WCHAR));
    WCHAR* pBegin = pTemp;

    hr = getValue(pszValueName, REG_MULTI_SZ, (LPBYTE)pTemp, cb);
    if (SUCCEEDED(hr)) {
        while (pTemp && TEXT('\0') != *pTemp) {
            vStrValues.push_back(std::wstring(pTemp));
            pTemp += lstrlenW(pTemp) + 1;
        }
    }

    SAFE_FREE(pBegin);

    return hr;
}

HRESULT WinRegistry::getValueBufferSize(LPCWSTR pszValueName, DWORD& dwSize) const noexcept {
    DWORD dwType;
    DWORD cbData = 0;
    LSTATUS ls = RegQueryValueExW(m_hkey, pszValueName, 0, &dwType, NULL, (LPDWORD)&cbData);
    if (ls == ERROR_SUCCESS) {
        dwSize = cbData;
    }
    return HRESULT_FROM_WIN32(ls);
}

HRESULT WinRegistry::setDWORDValue(LPCWSTR pszValueName, DWORD dwData) noexcept {
    return setValue(pszValueName, REG_DWORD, (const LPBYTE)&dwData, sizeof(dwData));
}

HRESULT WinRegistry::setBINARYValue(LPCWSTR pszValueName, const LPBYTE pbData, int cbData) noexcept {
    return setValue(pszValueName, REG_BINARY, pbData, cbData);
}

HRESULT WinRegistry::setSZValue(LPCWSTR pszValueName, const std::wstring& strData) noexcept {
    return setValue(pszValueName, REG_SZ, (const LPBYTE)strData.c_str(),
                    ((DWORD)strData.length()) * sizeof(WCHAR));
}

HRESULT WinRegistry::setExpandSZValue(LPCWSTR pszValueName, const std::wstring& strData) noexcept {
    return setValue(pszValueName, REG_EXPAND_SZ, (const LPBYTE)strData.c_str(),
                    ((DWORD)strData.length()) * sizeof(WCHAR));
}

HRESULT WinRegistry::setMultiSZValue(LPCWSTR pszValueName, const std::vector<std::wstring>& vStrValues) noexcept {
    WCHAR* ptrValues = createDoubleNulTermList(vStrValues);
    if (!ptrValues) {
        return E_OUTOFMEMORY;
    }

    size_t cch = 1;
    size_t n = vStrValues.size();

    for (size_t i = 0; i < n; i++)
        cch += vStrValues[i].length() + 1;

    HRESULT hr = setValue(pszValueName, REG_MULTI_SZ, (const LPBYTE)ptrValues, (DWORD)cch * sizeof(TCHAR));

    SAFE_DELETE_ARRAY(ptrValues);

    return hr;
}

HRESULT WinRegistry::getSubKeys(std::vector<std::wstring>& subKeys) noexcept {
    WCHAR achKey[256];               // buffer for subkey name
    DWORD cbName = 255;              // size of name string
    WCHAR achClass[MAX_PATH] = L"";  // buffer for class name
    DWORD cchClassName = MAX_PATH;   // size of class string
    DWORD cSubKeys = 0;              // number of subkeys
    DWORD cbMaxSubKey;               // longest subkey size
    DWORD cchMaxClass;               // longest class string
    DWORD cValues;                   // number of values for key
    DWORD cchMaxValue;               // longest value name
    DWORD cbMaxValueData;            // longest value data
    DWORD cbSecurityDescriptor;      // size of security descriptor
    FILETIME ftLastWriteTime;        // last write time

    subKeys.clear();

    LSTATUS ls = RegQueryInfoKeyW(m_hkey,                 // key handle
                                  achClass,               // buffer for class name
                                  &cchClassName,          // size of class string
                                  NULL,                   // reserved
                                  &cSubKeys,              // number of subkeys
                                  &cbMaxSubKey,           // longest subkey size
                                  &cchMaxClass,           // longest class string
                                  &cValues,               // number of values for this key
                                  &cchMaxValue,           // longest value name
                                  &cbMaxValueData,        // longest value data
                                  &cbSecurityDescriptor,  // security descriptor
                                  &ftLastWriteTime);      // last write time

    if (ls != ERROR_SUCCESS)
        return HRESULT_FROM_WIN32(ls);

    for (DWORD i = 0; i < cSubKeys; i++) {
        cbName = 255;
        ls = RegEnumKeyExW(m_hkey, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);
        if (ls != ERROR_SUCCESS) {
            break;
        }

        subKeys.push_back(achKey);
    }

    return HRESULT_FROM_WIN32(ls);
}

void WinRegistry::OnChange(HKEY hkey) noexcept {
    UNREFERENCED_PARAMETER(hkey);
    //
    // Default does nothing.
    //
}

HRESULT WinRegistry::getValue(LPCWSTR pszValueName, DWORD dwTypeExpected, LPBYTE pbData, DWORD cbData) const noexcept {
    DWORD dwType = 0;
    LSTATUS ls = RegQueryValueExW(m_hkey, pszValueName, 0, &dwType, pbData, (LPDWORD)&cbData);

    if (ERROR_SUCCESS == ls && dwType != dwTypeExpected)
        ls = ERROR_INVALID_DATATYPE;

    return HRESULT_FROM_WIN32(ls);
}

HRESULT WinRegistry::setValue(LPCWSTR pszValueName, DWORD dwValueType, const LPBYTE pbData, DWORD cbData) noexcept {
    LSTATUS ls = RegSetValueExW(m_hkey, pszValueName, 0, dwValueType, pbData, cbData);
    return HRESULT_FROM_WIN32(ls);
}

LPWSTR WinRegistry::createDoubleNulTermList(const std::vector<std::wstring>& vStrValues) const noexcept {
    size_t cEntries = vStrValues.size();
    size_t cch = 1;  // Account for 2nd null terminate.

    for (size_t i = 0; i < cEntries; i++)
        cch += vStrValues[i].length() + 1;

    LPWSTR pszBuf = new (std::nothrow) WCHAR[cch];
    if (!pszBuf)
        return NULL;

    LPWSTR pszWrite = pszBuf;

    for (size_t i = 0; i < cEntries; i++) {
        const std::wstring& s = vStrValues[i];
        StringCchCopyW(pszWrite, cch, s.c_str());
        pszWrite += s.length() + 1;
    }

    *pszWrite = L'\0';  // Double null terminate.
    return pszBuf;
}

unsigned int _stdcall WinRegistry::NotifyWaitThreadProc(LPVOID pvParam) {
    WinRegistry* pThis = (WinRegistry*)pvParam;

    while (NULL != pThis->m_hkey) {
        LONG lResult = RegNotifyChangeKeyValue(pThis->m_hkey, pThis->m_bWatchSubtree,
                                               pThis->m_dwChangeFilter, pThis->m_hChangeEvent, true);

        if (ERROR_SUCCESS != lResult) {
            return 0;
        }
        else {
            switch (WaitForSingleObject(pThis->m_hChangeEvent, INFINITE)) {
                case WAIT_OBJECT_0:
                    if (NULL != pThis->m_hkey) {
                        pThis->OnChange(pThis->m_hkey);
                    }

                    break;

                case WAIT_FAILED:
                    break;

                default:
                    break;
            }
        }
    }

    return 0;
}

bool WinRegistry::regDeleteKey32_64(HKEY hKey, LPCWSTR pszSubKey, bool bPrefer64View) noexcept {
    REGSAM rsam = (bPrefer64View && OSVersion::IsWin64()) ? KEY_WOW64_64KEY : KEY_WOW64_32KEY;
    HMODULE hAdvAPI32 = LoadLibrary(TEXT("AdvAPI32.dll"));

    if (!hAdvAPI32)
        return false;

    LSTATUS ls;
    typedef LONG(WINAPI * PFN_RegDeleteKeyEx)(HKEY hKey, LPCWSTR lpSubKey, REGSAM samDesired,
                                              DWORD Reserved);
#if (defined UNICODE) || (defined _UNICODE)
#define RegDeleteKeyExFuncName "RegDeleteKeyExW"
#else
#define RegDeleteKeyExFuncName "RegDeleteKeyExA"
#endif
    PFN_RegDeleteKeyEx _RegDeleteKeyEx =
        (PFN_RegDeleteKeyEx)GetProcAddress(hAdvAPI32, RegDeleteKeyExFuncName);

    if (_RegDeleteKeyEx) {
        ls = _RegDeleteKeyEx(hKey, pszSubKey, rsam, 0);
        FreeLibrary(hAdvAPI32);
    }
    else {
        ls = RegDeleteKeyW(hKey, pszSubKey);
    }

    return (ls == ERROR_SUCCESS);
}

bool WinRegistry::regDeleteSubKeys(HKEY hKey, bool bPrefer64View) noexcept {
    DWORD dwSubKeyCnt, dwMaxSubKey;
    const int iMaxKeySize = 256;

    if (RegQueryInfoKeyW(hKey, NULL, NULL, 0, &dwSubKeyCnt, &dwMaxSubKey, NULL, NULL, NULL, NULL,
                         NULL, NULL) == ERROR_SUCCESS) {
        if (dwSubKeyCnt) {
            dwMaxSubKey += sizeof(WCHAR);
            WCHAR szKeyName[iMaxKeySize] = {0};

            do {
                if (RegEnumKeyW(hKey, --dwSubKeyCnt, szKeyName, iMaxKeySize) == ERROR_SUCCESS) {
                    HKEY hSubKey = NULL;

                    if (ERROR_SUCCESS ==
                            RegOpenKeyExW(hKey, szKeyName, 0, KEY_READ | KEY_WRITE, &hSubKey) &&
                        hSubKey) {
                        if (regDeleteSubKeys(hSubKey, bPrefer64View)) {
                            if (hSubKey)
                                RegCloseKey(hSubKey);

                            if (!regDeleteKey32_64(hKey, szKeyName, bPrefer64View))
                                return false;
                        }
                        else {
                            if (hSubKey)
                                RegCloseKey(hSubKey);

                            return false;
                        }
                    }
                    else {
                        return false;
                    }
                }
                else {
                    return false;
                }
            } while (dwSubKeyCnt);
        }

        return true;
    }

    return false;
}

BOOL WinRegistry::regDelSubKeysRecurse(HKEY hKeyRoot, LPTSTR lpSubKey, bool bPrefer64View) noexcept {
    LPTSTR lpEnd = NULL;
    LONG lResult;
    DWORD dwSize = 0;
    TCHAR szName[MAX_PATH] = {0};
    HKEY hKey = NULL;
    FILETIME ftWrite;

    // First, see if we can delete the key without having to recurse.
    lResult =
        RegDeleteKeyEx(hKeyRoot, lpSubKey, bPrefer64View ? KEY_WOW64_64KEY : KEY_WOW64_32KEY, 0);
    if (lResult == ERROR_SUCCESS)
        return TRUE;

    REGSAM rsam = KEY_READ;
    if (bPrefer64View)
        rsam |= KEY_WOW64_64KEY;

    lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, rsam, &hKey);
    if (lResult != ERROR_SUCCESS) {
        if (lResult == ERROR_FILE_NOT_FOUND) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    // Check for an ending slash and add one if it is missing.

    lpEnd = lpSubKey + lstrlen(lpSubKey);

    if (*(lpEnd - 1) != TEXT('\\')) {
        *lpEnd = TEXT('\\');
        lpEnd++;
        *lpEnd = TEXT('\0');
    }

    // Enumerate the keys
    dwSize = MAX_PATH;
    lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
    if (lResult == ERROR_SUCCESS) {
        do {
            *lpEnd = TEXT('\0');
            StringCchCat(lpSubKey, MAX_PATH * 2, szName);

            if (!regDelSubKeysRecurse(hKeyRoot, lpSubKey, bPrefer64View)) {
                break;
            }

            dwSize = MAX_PATH;
            lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
        } while (lResult == ERROR_SUCCESS);
    }

    lpEnd--;
    *lpEnd = TEXT('\0');

    RegCloseKey(hKey);

    // Try again to delete the key.
    lResult =
        RegDeleteKeyEx(hKeyRoot, lpSubKey, bPrefer64View ? KEY_WOW64_64KEY : KEY_WOW64_32KEY, 0);
    if (lResult == ERROR_SUCCESS)
        return TRUE;

    return FALSE;
}
}  // namespace ashe
#endif