#include "ashe/config.h"
#include "ashe/win/ini.h"
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
#include "ashe/scoped_clear_last_error.h"

namespace ashe {
namespace win {
Ini::Ini(const std::wstring& filePath) :
    iniFilePath_(filePath) {
}

void Ini::setIniFilePath(const std::wstring& filePath) {
    iniFilePath_ = filePath;
}

std::wstring Ini::iniFilePath() const {
    return iniFilePath_;
}

bool Ini::readInt(const std::wstring& item, const std::wstring& subItem, unsigned int& result) {
    if (iniFilePath_.empty())
        return false;

    ScopedClearLastError scle;
    INT iDefault = 0;
    UINT ret = GetPrivateProfileIntW(item.c_str(), subItem.c_str(), iDefault, iniFilePath_.c_str());
    DWORD dwGLE = GetLastError();
    if (dwGLE == 0) {
        result = ret;
        return true;
    }
    return false;
}

unsigned int Ini::readIntWithDefault(const std::wstring& item,
                                     const std::wstring& subItem,
                                     unsigned int defaultValue) {
    if (iniFilePath_.empty())
        return defaultValue;

    ScopedClearLastError scle;
    return GetPrivateProfileIntW(item.c_str(), subItem.c_str(), defaultValue,
                                 iniFilePath_.c_str());
}

std::wstring Ini::readStringWithDefault(const std::wstring& item,
                                        const std::wstring& subItem,
                                        const std::wstring& defaultValue) {
    std::wstring result;
    if (!readString(item, subItem, result)) {
        result = defaultValue;
    }

    return result;
}

bool Ini::readString(const std::wstring& item,
                     const std::wstring& subItem,
                     std::wstring& result) {
    if (iniFilePath_.empty())
        return false;

    ScopedClearLastError scle;
    bool ret = false;
    int iBufSize = 255;
    WCHAR* pBuf = NULL;
    do {
        pBuf = (WCHAR*)malloc(iBufSize * sizeof(WCHAR));
        if (!pBuf) {
            break;
        }
        memset(pBuf, 0, iBufSize * sizeof(WCHAR));
        DWORD dwRet = GetPrivateProfileStringW(item.c_str(), subItem.c_str(), L"", pBuf, iBufSize, iniFilePath_.c_str());
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

bool Ini::writeInt(const std::wstring& item, const std::wstring& sub_item, unsigned int value) {
    if (iniFilePath_.empty())
        return false;

    WCHAR szValue[50];
    StringCchPrintfW(szValue, 50, L"%ld", value);
    return writeString(item, sub_item, szValue);
}

bool Ini::writeString(const std::wstring& item,
                      const std::wstring& sub_item,
                      const std::wstring& value) {
    if (iniFilePath_.empty())
        return false;

    return !!WritePrivateProfileStringW(item.c_str(), sub_item.c_str(), value.c_str(),
                                        iniFilePath_.c_str());
}
}  // namespace win
}  // namespace ashe