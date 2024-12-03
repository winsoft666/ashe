#include "ashe/win/file_property.h"
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

namespace ashe {
namespace win {
FileProperty::FileProperty() {
    reset();
}

bool FileProperty::getTranslationId(LPVOID lpData,
                                    UINT unBlockSize,
                                    WORD wLangId,
                                    DWORD& dwId,
                                    BOOL bPrimaryEnough /*= FALSE*/) {
    LPWORD lpwData;

    for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData) + unBlockSize; lpwData += 2) {
        if (*lpwData == wLangId) {
            dwId = *((DWORD*)lpwData);
            return true;
        }
    }

    if (!bPrimaryEnough)
        return false;

    for (lpwData = (LPWORD)lpData; (LPBYTE)lpwData < ((LPBYTE)lpData) + unBlockSize; lpwData += 2) {
        if (((*lpwData) & 0x00FF) == (wLangId & 0x00FF)) {
            dwId = *((DWORD*)lpwData);
            return true;
        }
    }

    return false;
}

bool FileProperty::open(HMODULE hModule) {
    WCHAR szBuf[MAX_PATH + 1] = {0};
    GetModuleFileNameW(hModule, szBuf, MAX_PATH);

    return open(szBuf);
}

bool FileProperty::open(const std::wstring& strFileName) {
    reset();

    HANDLE hFile = CreateFileW(strFileName.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                               NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    BOOL bRet = GetFileTime(hFile, &m_ftCreationTime, &m_ftLastAccessTime, &m_ftLastWriteTime);
    if (!bRet) {
        CloseHandle(hFile);
        return false;
    }
    CloseHandle(hFile);
    hFile = INVALID_HANDLE_VALUE;

    DWORD dwHandle;
    DWORD dwFileVersionInfoSize = GetFileVersionInfoSizeW(strFileName.c_str(), &dwHandle);

    if (!dwFileVersionInfoSize)
        return false;

    LPVOID lpData = (LPVOID)(new BYTE[dwFileVersionInfoSize]);

    if (!lpData)
        return false;

    try {
        if (!GetFileVersionInfoW((LPTSTR)strFileName.c_str(), dwHandle, dwFileVersionInfoSize, lpData))
            return false;

        LPVOID lpInfo;
        UINT unInfoLen;

        if (VerQueryValueW(lpData, L"\\", &lpInfo, &unInfoLen)) {
            if (unInfoLen == sizeof(m_FileInfo))
                memcpy(&m_FileInfo, lpInfo, unInfoLen);
        }

        VerQueryValueW(lpData, L"\\VarFileInfo\\Translation", &lpInfo, &unInfoLen);

        DWORD dwLangCode = 0;

        if (!getTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, FALSE)) {
            if (!getTranslationId(lpInfo, unInfoLen, GetUserDefaultLangID(), dwLangCode, TRUE)) {
                if (!getTranslationId(lpInfo, unInfoLen, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
                                      dwLangCode, TRUE)) {
                    if (!getTranslationId(lpInfo, unInfoLen, MAKELANGID(LANG_ENGLISH, SUBLANG_NEUTRAL),
                                          dwLangCode, TRUE))
                        // use the first one we can get
                        dwLangCode = *((DWORD*)lpInfo);
                }
            }
        }

        WCHAR strSubBlock[MAX_PATH * 2] = {0};
        ZeroMemory(strSubBlock, MAX_PATH * 2 * sizeof(WCHAR));
        swprintf_s(strSubBlock, L"\\StringFileInfo\\%04X%04X\\", dwLangCode & 0x0000FFFF,
                   (dwLangCode & 0xFFFF0000) >> 16);

        WCHAR strTmp[MAX_PATH * 2] = {0};
        ZeroMemory(strTmp, MAX_PATH * 2 * sizeof(WCHAR));

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"CompanyName");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strCompanyName = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"FileDescription");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strFileDescription = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"FileVersion");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strFileVersion = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"InternalName");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strInternalName = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"LegalCopyright");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strLegalCopyright = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"OriginalFileName");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strOriginalFileName = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"ProductName");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strProductName = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"ProductVersion");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strProductVersion = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"Comments");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strComments = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"LegalTrademarks");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strLegalTrademarks = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"PrivateBuild");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strPrivateBuild = (LPCTSTR)lpInfo;

        swprintf_s(strTmp, L"%s%s", strSubBlock, L"SpecialBuild");

        if (VerQueryValueW(lpData, strTmp, &lpInfo, &unInfoLen))
            m_strSpecialBuild = (LPCTSTR)lpInfo;

        delete[] lpData;
    } catch (...) {
        delete[] lpData;
        return false;
    }

    return true;
}

WORD FileProperty::getFileVersion(int nIndex) const {
    switch (nIndex) {
        case 0:
            return (WORD)(m_FileInfo.dwFileVersionLS & 0x0000FFFF);

        case 1:
            return (WORD)((m_FileInfo.dwFileVersionLS & 0xFFFF0000) >> 16);

        case 2:
            return (WORD)(m_FileInfo.dwFileVersionMS & 0x0000FFFF);

        case 3:
            return (WORD)((m_FileInfo.dwFileVersionMS & 0xFFFF0000) >> 16);

        default:
            return 0;
    }
}

WORD FileProperty::getProductVersion(int nIndex) const {
    switch (nIndex) {
        case 0:
            return (WORD)(m_FileInfo.dwProductVersionLS & 0x0000FFFF);

        case 1:
            return (WORD)((m_FileInfo.dwProductVersionLS & 0xFFFF0000) >> 16);

        case 2:
            return (WORD)(m_FileInfo.dwProductVersionMS & 0x0000FFFF);

        case 3:
            return (WORD)((m_FileInfo.dwProductVersionMS & 0xFFFF0000) >> 16);

        default:
            return 0;
    }
}

DWORD FileProperty::getFileFlagsMask() const {
    return m_FileInfo.dwFileFlagsMask;
}

DWORD FileProperty::getFileFlags() const {
    return m_FileInfo.dwFileFlags;
}

DWORD FileProperty::getFileOs() const {
    return m_FileInfo.dwFileOS;
}

DWORD FileProperty::getFileType() const {
    return m_FileInfo.dwFileType;
}

DWORD FileProperty::getFileSubtype() const {
    return m_FileInfo.dwFileSubtype;
}

std::wstring FileProperty::getCompanyName() {
    return m_strCompanyName;
}

std::wstring FileProperty::getFileDescription() {
    return m_strFileDescription;
}

std::wstring FileProperty::getFileVersion() {
    return m_strFileVersion;
}

std::wstring FileProperty::getFileVersionEx() {
    wchar_t strTemp[MAX_PATH * 2] = {0};

    StringCchPrintfW(
        strTemp, MAX_PATH * 2, L"%d.%d.%d.%d", (m_FileInfo.dwFileVersionMS & 0xFFFF0000) >> 16,
        (m_FileInfo.dwFileVersionMS & 0x0000FFFF), (m_FileInfo.dwFileVersionLS & 0xFFFF0000) >> 16,
        m_FileInfo.dwFileVersionLS & 0x0000FFFF);

    return strTemp;
}

std::wstring FileProperty::getInternalName() {
    return m_strInternalName;
}

std::wstring FileProperty::getLegalCopyright() {
    return m_strLegalCopyright;
}

std::wstring FileProperty::getOriginalFileName() {
    return m_strOriginalFileName;
}

std::wstring FileProperty::getProductName() {
    return m_strProductName;
}

std::wstring FileProperty::getProductVersion() {
    return m_strProductVersion;
}

std::wstring FileProperty::getComments() {
    return m_strComments;
}

std::wstring FileProperty::getLegalTrademarks() {
    return m_strLegalTrademarks;
}

std::wstring FileProperty::getPrivateBuild() {
    return m_strPrivateBuild;
}

std::wstring FileProperty::getSpecialBuild() {
    return m_strSpecialBuild;
}

FILETIME FileProperty::getCreationTime() const {
    return m_ftCreationTime;
}

FILETIME FileProperty::getLastAccessTime() const {
    return m_ftLastAccessTime;
}

FILETIME FileProperty::getLastWriteTime() const {
    return m_ftLastWriteTime;
}

void FileProperty::reset() {
    ZeroMemory(&m_FileInfo, sizeof(m_FileInfo));
    m_strCompanyName.clear();
    m_strFileDescription.clear();
    m_strFileVersion.clear();
    m_strInternalName.clear();
    m_strLegalCopyright.clear();
    m_strOriginalFileName.clear();
    m_strProductName.clear();
    m_strProductVersion.clear();
    m_strComments.clear();
    m_strLegalTrademarks.clear();
    m_strPrivateBuild.clear();
    m_strSpecialBuild.clear();

    m_ftCreationTime.dwHighDateTime = m_ftCreationTime.dwLowDateTime = 0;
    m_ftLastAccessTime.dwHighDateTime = m_ftLastAccessTime.dwLowDateTime = 0;
    m_ftLastWriteTime.dwHighDateTime = m_ftLastWriteTime.dwLowDateTime = 0;
}
}  // namespace win
}  // namespace ashe