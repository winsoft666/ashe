#include "ashe/config.h"
#include "ashe/win_registry.h"
#ifdef ASHE_WIN
#include <process.h>
#include <strsafe.h>
#include "ashe/os_version.h"

namespace ashe {
WinRegistry::WinRegistry(HKEY hkeyRoot, const std::wstring& subKey) noexcept :
    hkeyRoot_(hkeyRoot),
    hkey_(NULL),
    hChangeEvent_(NULL),
    hNotifyThread_(NULL),
    bWatchSubtree_(false),
    dwChangeFilter_(0),
    dwSamDesired_(0),
    strSubKey_(subKey) {}

WinRegistry::~WinRegistry() noexcept {
    close();

    if (hChangeEvent_)
        CloseHandle(hChangeEvent_);
}

LSTATUS WinRegistry::open(REGSAM samDesired, bool bCreate) noexcept {
    LSTATUS dwResult;
    close();

    dwSamDesired_ = samDesired;
    if (bCreate) {
        DWORD dwDisposition;
        dwResult = RegCreateKeyExW(hkeyRoot_, strSubKey_.c_str(), 0, NULL, 0, samDesired, NULL,
                                   &hkey_, &dwDisposition);
    }
    else {
        dwResult = RegOpenKeyExW(hkeyRoot_, strSubKey_.c_str(), 0, samDesired, &hkey_);
    }

    return dwResult;
}

bool WinRegistry::isOpen() const noexcept {
    return NULL != hkey_;
}

HKEY WinRegistry::getHandle() const noexcept {
    return hkey_;
}

void WinRegistry::close() noexcept {
    if (NULL != hkey_) {
        HKEY hkeyTemp = hkey_;
        hkey_ = NULL;
        RegCloseKey(hkeyTemp);
    }

    if (hNotifyThread_) {
        WaitForSingleObject(hNotifyThread_, INFINITE);
    }

    if (hNotifyThread_) {
        CloseHandle(hNotifyThread_);
        hNotifyThread_ = NULL;
    }

    dwSamDesired_ = 0;
}

bool WinRegistry::watchForChange(DWORD dwChangeFilter, bool bWatchSubtree) noexcept {
    if (NULL != hChangeEvent_ || NULL == hkey_)
        return false;

    hChangeEvent_ = CreateEventW(NULL, FALSE, FALSE, NULL);

    if (NULL == hChangeEvent_) {
        return false;
    }

    dwChangeFilter_ = dwChangeFilter;
    bWatchSubtree_ = bWatchSubtree;

    unsigned int uThreadId = 0;
    hNotifyThread_ = (HANDLE)_beginthreadex(NULL, 0, NotifyWaitThreadProc, this, 0, &uThreadId);

    return !!hNotifyThread_;
}

LSTATUS WinRegistry::waitForChange(DWORD dwChangeFilter, bool bWatchSubtree) noexcept {
    LSTATUS ls = RegNotifyChangeKeyValue(hkey_, bWatchSubtree, dwChangeFilter, NULL, FALSE);
    return ls;
}

LSTATUS WinRegistry::DeleteKey(HKEY hKey, LPCWSTR pszSubKey, LPCWSTR pszValName, bool bPrefer64View) noexcept {
    HKEY hSubKey = NULL;
    LSTATUS status;

    if (!hKey) {
        return E_FAIL;
    }

    if (pszSubKey) {
        REGSAM rsam = KEY_READ | KEY_WRITE;

        if (bPrefer64View)
            rsam |= KEY_WOW64_64KEY;

        status = RegOpenKeyExW(hKey, pszSubKey, 0, rsam, &hSubKey);
        if (status != ERROR_SUCCESS) {
            return status;
        }
    }
    else {
        hSubKey = hKey;
    }

    if (pszValName) {
        status = RegDeleteValueW(hSubKey, pszValName);
    }
    else {
        status = regDeleteSubKeys(hSubKey, bPrefer64View);
        if (status == ERROR_SUCCESS) {
            status = regDeleteKey32_64(hKey, pszSubKey, bPrefer64View);
        }
    }

    if (hSubKey != hKey) {
        if (hSubKey)
            RegCloseKey(hSubKey);
    }

    return status;
}

LSTATUS WinRegistry::DeleteSubKeys(HKEY hKeyRoot, LPCWSTR lpSubKey, bool bPrefer64View) noexcept {
    WCHAR szDelKey[MAX_PATH * 2];

    StringCchCopyW(szDelKey, MAX_PATH * 2, lpSubKey);
    LSTATUS ls = regDelSubKeysRecurse(hKeyRoot, szDelKey, bPrefer64View);

    return ls;
}

LSTATUS WinRegistry::getDWORDValue(LPCWSTR pszValueName, OUT DWORD& pdwDataOut) const noexcept {
    return getValue(pszValueName, REG_DWORD, (LPBYTE)(&pdwDataOut), sizeof(DWORD));
}

LSTATUS WinRegistry::getBINARYValue(LPCWSTR pszValueName, LPBYTE pbDataOut, int cbDataOut) const noexcept {
    return getValue(pszValueName, REG_BINARY, pbDataOut, cbDataOut);
}

LSTATUS WinRegistry::getSZValue(LPCWSTR pszValueName, OUT std::wstring& strValue) const noexcept {
    DWORD cb = 0;
    LSTATUS ls = getValueBufferSize(pszValueName, cb);
    if (ls != ERROR_SUCCESS) {
        return ls;
    }

    if (cb == 0) {
        strValue = L"";
        return ERROR_SUCCESS;
    }

    WCHAR* pTemp = (WCHAR*)malloc(cb + sizeof(WCHAR));
    if (!pTemp)
        return ERROR_OUTOFMEMORY;

    memset(pTemp, 0, cb + sizeof(WCHAR));

    ls = getValue(pszValueName, REG_SZ, (LPBYTE)pTemp, cb);
    if (ls == ERROR_SUCCESS)
        strValue = pTemp;

    SAFE_FREE(pTemp);

    return ls;
}

LSTATUS WinRegistry::getExpandSZValue(LPCWSTR pszValueName,
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

    if (dwSamDesired_ & KEY_WOW64_64KEY)
        dwFlags |= RRF_SUBKEY_WOW6464KEY;
    else if (dwSamDesired_ & KEY_WOW64_32KEY)
        dwFlags |= RRF_SUBKEY_WOW6432KEY;
    else
        dwFlags |= RRF_SUBKEY_WOW6432KEY;

    LSTATUS status = RegGetValueW(hkeyRoot_, strSubKey_.c_str(), pszValueName, dwFlags, NULL, szBuf, &dwSize);
    if (status == ERROR_MORE_DATA) {
        WCHAR* pBuf = new (std::nothrow) WCHAR[dwSize / sizeof(WCHAR)];
        if (pBuf) {
            memset(pBuf, 0, dwSize);
            status = RegGetValueW(hkeyRoot_, strSubKey_.c_str(), pszValueName, dwFlags, NULL, pBuf, &dwSize);

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

    return status;
}

LSTATUS WinRegistry::getMultiSZValue(LPCWSTR pszValueName, OUT std::vector<std::wstring>& vStrValues) const noexcept {
    DWORD cb = 0;
    LSTATUS ls = getValueBufferSize(pszValueName, cb);

    if (ls != ERROR_SUCCESS) {
        return ls;
    }

    if (cb == 0) {
        vStrValues.clear();
        return ERROR_SUCCESS;
    }

    WCHAR* pTemp = (WCHAR*)malloc(cb + sizeof(WCHAR));
    if (!pTemp)
        return ERROR_OUTOFMEMORY;

    memset(pTemp, 0, cb + sizeof(WCHAR));
    WCHAR* pBegin = pTemp;

    ls = getValue(pszValueName, REG_MULTI_SZ, (LPBYTE)pTemp, cb);
    if (ls == ERROR_SUCCESS) {
        while (pTemp && TEXT('\0') != *pTemp) {
            vStrValues.push_back(std::wstring(pTemp));
            pTemp += lstrlenW(pTemp) + 1;
        }
    }

    SAFE_FREE(pBegin);

    return ls;
}

LSTATUS WinRegistry::getValueBufferSize(LPCWSTR pszValueName, DWORD& dwSize) const noexcept {
    DWORD dwType;
    DWORD cbData = 0;
    LSTATUS ls = RegQueryValueExW(hkey_, pszValueName, 0, &dwType, NULL, (LPDWORD)&cbData);
    if (ls == ERROR_SUCCESS) {
        dwSize = cbData;
    }
    return ls;
}

LSTATUS WinRegistry::setDWORDValue(LPCWSTR pszValueName, DWORD dwData) noexcept {
    return setValue(pszValueName, REG_DWORD, (const LPBYTE)&dwData, sizeof(dwData));
}

LSTATUS WinRegistry::setBINARYValue(LPCWSTR pszValueName, const LPBYTE pbData, int cbData) noexcept {
    return setValue(pszValueName, REG_BINARY, pbData, cbData);
}

LSTATUS WinRegistry::setSZValue(LPCWSTR pszValueName, const std::wstring& strData) noexcept {
    return setValue(pszValueName, REG_SZ, (const LPBYTE)strData.c_str(),
                    ((DWORD)strData.length() + 1) * sizeof(WCHAR));
}

LSTATUS WinRegistry::setExpandSZValue(LPCWSTR pszValueName, const std::wstring& strData) noexcept {
    return setValue(pszValueName, REG_EXPAND_SZ, (const LPBYTE)strData.c_str(),
                    ((DWORD)strData.length() + 1) * sizeof(WCHAR));
}

LSTATUS WinRegistry::setMultiSZValue(LPCWSTR pszValueName, const std::vector<std::wstring>& vStrValues) noexcept {
    WCHAR* ptrValues = createDoubleNulTermList(vStrValues);
    if (!ptrValues) {
        return ERROR_OUTOFMEMORY;
    }

    size_t cch = 1;
    size_t n = vStrValues.size();

    for (size_t i = 0; i < n; i++)
        cch += vStrValues[i].length() + 1;

    LSTATUS hr = setValue(pszValueName, REG_MULTI_SZ, (const LPBYTE)ptrValues, (DWORD)cch * sizeof(TCHAR));

    SAFE_DELETE_ARRAY(ptrValues);

    return hr;
}

LSTATUS WinRegistry::getSubKeys(std::vector<std::wstring>& subKeys) noexcept {
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

    LSTATUS ls = RegQueryInfoKeyW(hkey_,                  // key handle
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
        return ls;

    for (DWORD i = 0; i < cSubKeys; i++) {
        cbName = 255;
        ls = RegEnumKeyExW(hkey_, i, achKey, &cbName, NULL, NULL, NULL, &ftLastWriteTime);
        if (ls != ERROR_SUCCESS) {
            break;
        }

        subKeys.push_back(achKey);
    }

    return ls;
}

void WinRegistry::OnChange(HKEY hkey) noexcept {
    UNREFERENCED_PARAMETER(hkey);
    //
    // Default does nothing.
    //
}

LSTATUS WinRegistry::getValue(LPCWSTR pszValueName, DWORD dwTypeExpected, LPBYTE pbData, DWORD cbData) const noexcept {
    DWORD dwType = 0;
    LSTATUS ls = RegQueryValueExW(hkey_, pszValueName, 0, &dwType, pbData, (LPDWORD)&cbData);

    if (ERROR_SUCCESS == ls && dwType != dwTypeExpected)
        ls = ERROR_INVALID_DATATYPE;

    return ls;
}

LSTATUS WinRegistry::setValue(LPCWSTR pszValueName, DWORD dwValueType, const LPBYTE pbData, DWORD cbData) noexcept {
    LSTATUS ls = RegSetValueExW(hkey_, pszValueName, 0, dwValueType, pbData, cbData);
    return ls;
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

    while (NULL != pThis->hkey_) {
        LONG lResult = RegNotifyChangeKeyValue(pThis->hkey_, pThis->bWatchSubtree_,
                                               pThis->dwChangeFilter_, pThis->hChangeEvent_, true);

        if (ERROR_SUCCESS != lResult)
            return 0;

        switch (WaitForSingleObject(pThis->hChangeEvent_, INFINITE)) {
            case WAIT_OBJECT_0:
                if (NULL != pThis->hkey_) {
                    pThis->OnChange(pThis->hkey_);
                }

                break;

            case WAIT_FAILED:
                break;

            default:
                break;
        }
    }

    return 0;
}

LSTATUS WinRegistry::regDeleteKey32_64(HKEY hKey, LPCWSTR pszSubKey, bool bPrefer64View) noexcept {
    REGSAM rsam = (bPrefer64View && OSVersion::IsWin64()) ? KEY_WOW64_64KEY : KEY_WOW64_32KEY;
    HMODULE hAdvAPI32 = LoadLibrary(TEXT("AdvAPI32.dll"));

    if (!hAdvAPI32)
        return ERROR_NOT_SUPPORTED;

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

    return ls;
}

LSTATUS WinRegistry::regDeleteSubKeys(HKEY hKey, bool bPrefer64View) noexcept {
    DWORD dwSubKeyCnt = 0, dwMaxSubKey = 0;
    const int iMaxKeySize = 256;
    LSTATUS ls = RegQueryInfoKeyW(hKey, NULL, NULL, 0, &dwSubKeyCnt, &dwMaxSubKey, NULL, NULL, NULL, NULL, NULL, NULL);

    if (ls != ERROR_SUCCESS) {
        return ls;
    }

    if (dwSubKeyCnt == 0) {
        return ERROR_SUCCESS;
    }

    dwMaxSubKey += sizeof(WCHAR);
    WCHAR szKeyName[iMaxKeySize] = {0};

    do {
        ls = RegEnumKeyW(hKey, --dwSubKeyCnt, szKeyName, iMaxKeySize);
        if (ls != ERROR_SUCCESS) {
            break;
        }

        HKEY hSubKey = NULL;
        ls = RegOpenKeyExW(hKey, szKeyName, 0, KEY_READ | KEY_WRITE, &hSubKey);
        if (ERROR_SUCCESS != ls || !hSubKey) {
            break;
        }

        ls = regDeleteSubKeys(hSubKey, bPrefer64View);
        if (hSubKey)
            RegCloseKey(hSubKey);

        if (ls != ERROR_SUCCESS) {
            break;
        }

        ls = regDeleteKey32_64(hKey, szKeyName, bPrefer64View);
        if (ls != ERROR_SUCCESS) {
            break;
        }
    } while (dwSubKeyCnt);

    return ls;
}

LSTATUS WinRegistry::regDelSubKeysRecurse(HKEY hKeyRoot, LPWSTR lpSubKey, bool bPrefer64View) noexcept {
    LPWSTR lpEnd = NULL;
    LSTATUS ls;
    DWORD dwSize = 0;
    WCHAR szName[MAX_PATH] = {0};
    HKEY hKey = NULL;
    FILETIME ftWrite;

    // First, see if we can delete the key without having to recurse.
    ls = RegDeleteKeyExW(hKeyRoot, lpSubKey, bPrefer64View ? KEY_WOW64_64KEY : KEY_WOW64_32KEY, 0);
    if (ls == ERROR_SUCCESS)
        return ls;

    REGSAM rsam = KEY_READ;
    if (bPrefer64View)
        rsam |= KEY_WOW64_64KEY;

    ls = RegOpenKeyExW(hKeyRoot, lpSubKey, 0, rsam, &hKey);
    if (ls != ERROR_SUCCESS) {
        return ls;
    }

    // Check for an ending slash and add one if it is missing.

    lpEnd = lpSubKey + lstrlenW(lpSubKey);

    if (*(lpEnd - 1) != L'\\') {
        *lpEnd = L'\\';
        lpEnd++;
        *lpEnd = L'\0';
    }

    // Enumerate the keys
    dwSize = MAX_PATH;
    ls = RegEnumKeyExW(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
    if (ls == ERROR_SUCCESS) {
        do {
            *lpEnd = L'\0';
            StringCchCatW(lpSubKey, MAX_PATH * 2, szName);

            if (regDelSubKeysRecurse(hKeyRoot, lpSubKey, bPrefer64View) != ERROR_SUCCESS) {
                break;
            }

            dwSize = MAX_PATH;
            ls = RegEnumKeyExW(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
        } while (ls == ERROR_SUCCESS);
    }

    lpEnd--;
    *lpEnd = L'\0';

    RegCloseKey(hKey);

    // Try again to delete the key.
    ls = RegDeleteKeyExW(hKeyRoot, lpSubKey, bPrefer64View ? KEY_WOW64_64KEY : KEY_WOW64_32KEY, 0);
    return ls;
}
}  // namespace ashe
#endif