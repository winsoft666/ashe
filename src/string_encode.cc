#include "ashe/config.hpp"
#include "ashe/string_encode.hpp"
#include "ashe/macros.hpp"

#include <codecvt>
#include <locale>
#ifdef ASHE_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif  // !_INC_WINDOWS
#endif  // ASHE_WIN

#pragma warning(disable : 4309)

namespace ashe {
#ifdef ASHE_WIN
std::string StringEncode::UnicodeToAnsi(const std::wstring& str, unsigned int code_page) noexcept {
    std::string strRes;
    int iSize = ::WideCharToMultiByte(code_page, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

    if (iSize == 0)
        return strRes;

    char* szBuf = new (std::nothrow) char[iSize];

    if (!szBuf)
        return strRes;

    memset(szBuf, 0, iSize);

    ::WideCharToMultiByte(code_page, 0, str.c_str(), -1, szBuf, iSize, NULL, NULL);

    strRes = szBuf;
    delete[] szBuf;

    return strRes;
}

std::wstring StringEncode::AnsiToUnicode(const std::string& str, unsigned int code_page) noexcept {
    std::wstring strRes;

    int iSize = ::MultiByteToWideChar(code_page, 0, str.c_str(), -1, NULL, 0);

    if (iSize == 0)
        return strRes;

    wchar_t* szBuf = new (std::nothrow) wchar_t[iSize];

    if (!szBuf)
        return strRes;

    memset(szBuf, 0, iSize * sizeof(wchar_t));

    ::MultiByteToWideChar(code_page, 0, str.c_str(), -1, szBuf, iSize);

    strRes = szBuf;
    delete[] szBuf;

    return strRes;
}

#endif

std::string StringEncode::UnicodeToUtf8(const std::wstring& str) noexcept {
    std::string strRes;
#ifdef ASHE_WIN

    int iSize = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

    if (iSize == 0)
        return strRes;

    char* szBuf = new (std::nothrow) char[iSize];

    if (!szBuf)
        return strRes;

    memset(szBuf, 0, iSize);

    ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize, NULL, NULL);

    strRes = szBuf;
    delete[] szBuf;
    return strRes;
#else
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
        strRes = converter.to_bytes(str);
    } catch (std::exception& e) {
        ASHE_UNUSED(e);
        strRes.clear();
    }
    return strRes;
#endif
}

std::string StringEncode::UnicodeToUtf8BOM(const std::wstring& str) noexcept {
#ifdef ASHE_WIN
    std::string strRes;

    const int iSize = ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

    if (iSize == 0)
        return strRes;

    char* szBuf = new (std::nothrow) char[iSize + 3];

    if (!szBuf)
        return strRes;

    memset(szBuf, 0, iSize + 3);

    ::WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, &szBuf[3], iSize, NULL, NULL);
    szBuf[0] = (char)0xef;
    szBuf[1] = (char)0xbb;
    szBuf[2] = (char)0xbf;

    strRes = szBuf;
    delete[] szBuf;

    return strRes;
#else
    std::string strRes;

    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
        const std::string stru8 = converter.to_bytes(str);

        strRes.resize(3);
        strRes[0] = (char)0xef;
        strRes[1] = (char)0xbb;
        strRes[2] = (char)0xbf;

        strRes += stru8;
    } catch (std::exception& e) {
        ASHE_UNUSED(e);
        strRes.clear();
    }
    return strRes;
#endif
}

std::wstring StringEncode::Utf8ToUnicode(const std::string& str) noexcept {
#ifdef ASHE_WIN
    std::wstring strRes;
    const int iSize = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

    if (iSize == 0)
        return strRes;

    wchar_t* szBuf = new (std::nothrow) wchar_t[iSize];

    if (!szBuf)
        return strRes;

    memset(szBuf, 0, iSize * sizeof(wchar_t));
    ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, szBuf, iSize);

    strRes = szBuf;
    delete[] szBuf;

    return strRes;
#else
    std::wstring strRes;

    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t> > converter;
        strRes = converter.from_bytes(str);
    } catch (std::exception& e) {
        ASHE_UNUSED(e);
        strRes.clear();
    }

    return strRes;
#endif
}

#ifdef ASHE_WIN
std::string StringEncode::AnsiToUtf8(const std::string& str, unsigned int code_page) noexcept {
    return UnicodeToUtf8(AnsiToUnicode(str, code_page));
}

std::string StringEncode::AnsiToUtf8BOM(const std::string& str, unsigned int code_page) noexcept {
    return UnicodeToUtf8BOM(AnsiToUnicode(str, code_page));
}

std::string StringEncode::Utf8ToAnsi(const std::string& str, unsigned int code_page) noexcept {
    return UnicodeToAnsi(Utf8ToUnicode(str), code_page);
}
#endif
}  // namespace ashe