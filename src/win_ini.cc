#include "ashe/config.hpp"
#include "ashe/win_ini.hpp"
#ifdef ASHE_WIN
#include <tchar.h>
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
#include <assert.h>
#include <vector>

namespace ashe {
WinIni::WinIni(const std::wstring& file_path) :
    iniFilePath_(file_path) {}

WinIni::WinIni(std::wstring&& file_path) :
    iniFilePath_(std::move(file_path)) {}

void WinIni::setIniFilePath(const std::wstring& file_path) noexcept {
    iniFilePath_ = file_path;
}

std::wstring WinIni::iniFilePath() const noexcept {
    return iniFilePath_;
}

bool WinIni::readInt(const std::wstring& item, const std::wstring& sub_item, unsigned int& result) noexcept {
    if (iniFilePath_.length() == 0)
        return false;
    INT iDefault = 0;
    SetLastError(0);
    UINT ret =
        GetPrivateProfileIntW(item.c_str(), sub_item.c_str(), iDefault, iniFilePath_.c_str());
    DWORD dwGLE = GetLastError();
    if (dwGLE == 0) {
        result = ret;
        return true;
    }
    return false;
}

UINT WinIni::readIntWithDefault(const std::wstring& item,
                                const std::wstring& subItem,
                                UINT defaultValue) noexcept {
    if (iniFilePath_.length() == 0)
        return defaultValue;

    SetLastError(0);
    return GetPrivateProfileIntW(item.c_str(), subItem.c_str(), defaultValue,
                                 iniFilePath_.c_str());
}

std::wstring WinIni::readStringWithDefault(const std::wstring& item,
                                           const std::wstring& subItem,
                                           const std::wstring& defaultValue) noexcept {
    std::wstring result;
    if (!readString(item, subItem, result)) {
        result = defaultValue;
    }

    return result;
}

bool WinIni::readString(const std::wstring& item,
                        const std::wstring& subItem,
                        std::wstring& result) noexcept {
    if (iniFilePath_.length() == 0)
        return false;

    bool ret = false;
    int iBufSize = 255;
    WCHAR* pBuf = NULL;
    do {
        pBuf = (WCHAR*)malloc(iBufSize * sizeof(WCHAR));
        if (!pBuf) {
            break;
        }
        memset(pBuf, 0, iBufSize * sizeof(WCHAR));
        SetLastError(0);
        DWORD dwRet = GetPrivateProfileStringW(item.c_str(), subItem.c_str(), L"", pBuf, iBufSize,
                                               iniFilePath_.c_str());
        DWORD dwGLE = GetLastError();
        if (dwRet == 0) {
            ret = (dwGLE == 0);
            break;
        }
        else if (dwRet == iBufSize - 1) {
            free(pBuf);
            pBuf = NULL;
            iBufSize += 1024;
        }
        else {
            ret = true;
            break;
        }
    } while (true);

    if (ret) {
        result = pBuf;
    }
    free(pBuf);

    return ret;
}

bool WinIni::writeInt(const std::wstring& item, const std::wstring& sub_item, unsigned int value) noexcept {
    if (iniFilePath_.length() == 0)
        return false;

    WCHAR szValue[50];
    StringCchPrintfW(szValue, 50, L"%ld", value);
    return writeString(item, sub_item, szValue);
}

bool WinIni::writeString(const std::wstring& item,
                         const std::wstring& sub_item,
                         const std::wstring& value) noexcept {
    if (iniFilePath_.length() == 0)
        return false;

    return !!WritePrivateProfileStringW(item.c_str(), sub_item.c_str(), value.c_str(),
                                        iniFilePath_.c_str());
}
}  // namespace ashe
#endif