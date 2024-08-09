#include "ashe/config.h"
#include "ashe/string_helper.h"
#include "ashe/macros.h"
#include "ashe/path_util.h"
#include <algorithm>
#include <cctype>
#include <cstdarg>
#include <cwctype>
#include <iterator>
#include <sstream>
#include <cassert>
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
#include <strsafe.h>
#endif

#if defined(__GNUC__)
#define VA_COPY(a, b) (va_copy(a, b))
#else
#define VA_COPY(a, b) (a = b)
#endif

#ifndef COUNT_OF
#define COUNT_OF(array) (sizeof(array) / sizeof(array[0]))
#endif

namespace ashe {
namespace stringhelper_detail {
struct EqualA {
    EqualA() = delete;
    EqualA(bool caseInsensitive) :
        caseInsensitive_(caseInsensitive) {}

    bool operator()(char ch1, char ch2) {
        if (caseInsensitive_)
            return ToUpper(ch1) == ToUpper(ch2);
        return ch1 == ch2;
    }

   private:
    bool caseInsensitive_ = false;
};

struct EqualW {
    EqualW() = delete;
    EqualW(bool caseInsensitive) :
        caseInsensitive_(caseInsensitive) {}

    bool operator()(wchar_t ch1, wchar_t ch2) {
        if (caseInsensitive_)
            return ToUpper(ch1) == ToUpper(ch2);
        return ch1 == ch2;
    }

   private:
    bool caseInsensitive_ = false;
};
}  // namespace stringhelper_detail
char ToLower(const char& in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

char ToUpper(const char& in) {
    if (in <= 'z' && in >= 'a')
        return in + ('Z' - 'z');
    return in;
}

wchar_t ToLower(const wchar_t& in) {
    if (in <= 'Z' && in >= 'A')
        return in - (L'Z' - L'z');
    return in;
}

wchar_t ToUpper(const wchar_t& in) {
    if (in <= L'z' && in >= L'a')
        return in + (L'Z' - L'z');
    return in;
}

std::string ToLower(const std::string& s) {
    std::string d = s;
    char (*pf)(const char&) = ToLower;
    std::transform(d.begin(), d.end(), d.begin(), pf);
    return d;
}

std::wstring ToLower(const std::wstring& s) {
    std::wstring d = s;
    wchar_t (*pf)(const wchar_t&) = ToLower;
    std::transform(d.begin(), d.end(), d.begin(), pf);
    return d;
}

std::string ToUpper(const std::string& s) {
    std::string d = s;
    char (*pf)(const char&) = ToUpper;
    std::transform(d.begin(), d.end(), d.begin(), pf);
    return d;
}

std::wstring ToUpper(const std::wstring& s) {
    std::wstring d = s;
    wchar_t (*pf)(const wchar_t&) = ToUpper;
    std::transform(d.begin(), d.end(), d.begin(), pf);
    return d;
}

bool IsDigit(const std::string& s) {
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
}

bool IsDigit(const std::wstring& s) {
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](wchar_t c) { return !std::iswdigit(c); }) == s.end();
}

bool IsLetterOrDigit(const char& c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool IsLetterOrDigit(const wchar_t& c) {
    return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z');
}

bool IsLetterOrDigit(const std::string& s) {
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](char c) { return !IsLetterOrDigit(c); }) == s.end();
}

bool IsLetterOrDigit(const std::wstring& s) {
    return !s.empty() &&
           std::find_if(s.begin(), s.end(), [](wchar_t c) { return !IsLetterOrDigit(c); }) == s.end();
}

std::string StrTrim(const std::string& s, const std::string& whitespaces) {
    const std::string::size_type pos = s.find_first_not_of(whitespaces);
    if (pos == std::string::npos) {
        return std::string();
    }

    std::string::size_type n = s.find_last_not_of(whitespaces) - pos + 1;
    return s.substr(pos, n);
}

std::wstring StrTrim(const std::wstring& s, const std::wstring& whitespaces) {
    const std::wstring::size_type pos = s.find_first_not_of(whitespaces);
    if (pos == std::wstring::npos) {
        return std::wstring();
    }

    std::wstring::size_type n = s.find_last_not_of(whitespaces) - pos + 1;
    return s.substr(pos, n);
}

std::string StrTrimLeft(const std::string& s, const std::string& whitespaces) {
    const std::string::size_type pos = s.find_first_not_of(whitespaces);
    if (pos == std::string::npos) {
        return std::string();
    }

    return s.substr(pos);
}

std::wstring StrTrimLeft(const std::wstring& s, const std::wstring& whitespaces) {
    const std::wstring::size_type pos = s.find_first_not_of(whitespaces);
    if (pos == std::wstring::npos) {
        return std::wstring();
    }

    return s.substr(pos);
}

std::string StrTrimRight(const std::string& s, const std::string& whitespaces) {
    const std::string::size_type pos = s.find_last_not_of(whitespaces);
    if (pos == std::string::npos) {
        return std::string();
    }

    return s.substr(0, pos + 1);
}

std::wstring StrTrimRight(const std::wstring& s, const std::wstring& whitespaces) {
    const std::wstring::size_type pos = s.find_last_not_of(whitespaces);
    if (pos == std::wstring::npos) {
        return std::wstring();
    }

    return s.substr(0, pos + 1);
}

bool IsStartsWith(const std::string& s, const std::string& prefix) {
    return s.compare(0, prefix.length(), prefix) == 0;
}

bool IsStartsWith(const std::wstring& s, const std::wstring& prefix) {
    return s.compare(0, prefix.length(), prefix) == 0;
}

bool IsEndsWith(const std::string& s, const std::string& suffix) {
    if (suffix.length() <= s.length()) {
        return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}

bool IsEndsWith(const std::wstring& s, const std::wstring& suffix) {
    if (suffix.length() <= s.length()) {
        return s.compare(s.length() - suffix.length(), suffix.length(), suffix) == 0;
    }
    return false;
}

bool IsContains(const std::string& str, const std::string& substring) {
    return (str.find(substring) != std::string::npos);
}

bool IsContains(const std::wstring& str, const std::wstring& substring) {
    return (str.find(substring) != std::wstring::npos);
}

size_t StrContainTimes(const std::string& str, const std::string& substring) {
    size_t times = 0;
    size_t pos = std::string::npos;
    size_t offset = 0;

    if (substring.length() == 0)
        return 0;

    do {
        pos = str.find(substring, offset);
        if (pos == std::string::npos)
            break;

        offset = pos + substring.length();
        times++;
    } while (true);

    return times;
}

size_t StrContainTimes(const std::wstring& str, const std::wstring& substring) {
    size_t times = 0;
    size_t pos = std::wstring::npos;
    size_t offset = 0;

    if (substring.length() == 0)
        return 0;

    do {
        pos = str.find(substring, offset);
        if (pos == std::wstring::npos)
            break;

        offset = pos + substring.length();
        times++;
    } while (true);

    return times;
}

std::string::size_type StrFind(const std::string& str, const std::string& substring, std::string::size_type offset, bool caseInsensitive) {
    if (offset >= str.length())
        return std::wstring::npos;

    std::string::const_iterator src_it = str.begin() + offset;
    std::string::const_iterator it = std::search(src_it,
                                                 str.end(),
                                                 substring.begin(),
                                                 substring.end(),
                                                 stringhelper_detail::EqualA(caseInsensitive));
    if (it != str.end())
        return it - str.begin();

    return std::string::npos;  // Not found
}

std::wstring::size_type StrFind(const std::wstring& str, const std::wstring& substring, std::wstring::size_type offset, bool caseInsensitive) {
    if (offset >= str.length())
        return std::wstring::npos;

    std::wstring::const_iterator src_it = str.begin() + offset;
    std::wstring::const_iterator it = std::search(src_it,
                                                  str.end(),
                                                  substring.begin(),
                                                  substring.end(),
                                                  stringhelper_detail::EqualW(caseInsensitive));
    if (it != str.end())
        return it - str.begin();

    return std::wstring::npos;  // Not found
}

std::string StrReplaceFirst(const std::string& s, const std::string& from, const std::string& to) {
    const size_t start_pos = s.find(from);
    if (start_pos == std::string::npos) {
        return s;
    }

    std::string ret = s;
    ret.replace(start_pos, from.length(), to);
    return ret;
}

std::wstring StrReplaceFirst(const std::wstring& s, const std::wstring& from, const std::wstring& to) {
    const size_t start_pos = s.find(from);
    if (start_pos == std::wstring::npos) {
        return s;
    }

    std::wstring ret = s;
    ret.replace(start_pos, from.length(), to);
    return ret;
}

std::string StrReplaceLast(const std::string& s, const std::string& from, const std::string& to) {
    const size_t start_pos = s.rfind(from);
    if (start_pos == std::string::npos) {
        return s;
    }

    std::string ret = s;
    ret.replace(start_pos, from.length(), to);
    return ret;
}

std::wstring StrReplaceLast(const std::wstring& s, const std::wstring& from, const std::wstring& to) {
    const size_t start_pos = s.rfind(from);
    if (start_pos == std::wstring::npos) {
        return s;
    }

    std::wstring ret = s;
    ret.replace(start_pos, from.length(), to);
    return ret;
}

std::string StrReplace(const std::string& s, const std::string& from, const std::string& to, std::wstring::size_type offset, bool caseInsensitive) {
    if (from.empty()) {
        return s;
    }

    const bool found_substring = StrFind(s, from, offset, caseInsensitive) != std::string::npos;
    if (!found_substring) {
        return s;
    }

    size_t start_pos = 0;
    std::string ret = s;
    while ((start_pos = StrFind(ret, from, start_pos, caseInsensitive)) != std::string::npos) {
        ret.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return ret;
}

std::wstring StrReplace(const std::wstring& s, const std::wstring& from, const std::wstring& to, std::wstring::size_type offset, bool caseInsensitive) {
    if (from.empty()) {
        return s;
    }

    const bool found_substring = StrFind(s, from, offset, caseInsensitive) != std::wstring::npos;
    if (!found_substring) {
        return s;
    }

    size_t start_pos = 0;
    std::wstring ret = s;
    while ((start_pos = StrFind(ret, from, start_pos, caseInsensitive)) != std::wstring::npos) {
        ret.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return ret;
}

std::vector<std::string> StrSplit(const std::string& src, const std::string& delimiter, bool includeEmptyStr) {
    std::vector<std::string> fields;
    typename std::string::size_type offset = 0;
    typename std::string::size_type pos = src.find(delimiter, 0);

    while (pos != std::string::npos) {
        std::string t = src.substr(offset, pos - offset);
        if ((t.length() > 0) || (t.length() == 0 && includeEmptyStr))
            fields.push_back(t);
        offset = pos + delimiter.length();
        pos = src.find(delimiter, offset);
    }

    const std::string t = src.substr(offset);
    if ((t.length() > 0) || (t.length() == 0 && includeEmptyStr))
        fields.push_back(t);
    return fields;
}

std::vector<std::wstring> StrSplit(const std::wstring& src, const std::wstring& delimiter, bool includeEmptyStr) {
    std::vector<std::wstring> fields;
    typename std::wstring::size_type offset = 0;
    typename std::wstring::size_type pos = src.find(delimiter, 0);

    while (pos != std::wstring::npos) {
        std::wstring t = src.substr(offset, pos - offset);
        if ((t.length() > 0) || (t.length() == 0 && includeEmptyStr))
            fields.push_back(t);
        offset = pos + delimiter.length();
        pos = src.find(delimiter, offset);
    }

    const std::wstring t = src.substr(offset);
    if ((t.length() > 0) || (t.length() == 0 && includeEmptyStr))
        fields.push_back(t);
    return fields;
}

std::string StrJoin(const std::vector<std::string>& src, const std::string& delimiter, bool includeEmptyStr) {
    std::stringstream ss;
    for (std::vector<std::string>::const_iterator it = src.cbegin(); it != src.cend(); ++it) {
        if (it->length() > 0) {
            ss << *it;
        }
        else {
            if (includeEmptyStr) {
                ss << *it;
            }
        }

        if (it + 1 != src.cend()) {
            ss << delimiter;
        }
    }
    return ss.str();
}

std::wstring StrJoin(const std::vector<std::wstring>& src, const std::wstring& delimiter, bool includeEmptyStr) {
    std::wstringstream ss;
    for (std::vector<std::wstring>::const_iterator it = src.cbegin(); it != src.cend(); ++it) {
        if (it->length() > 0) {
            ss << *it;
        }
        else {
            if (includeEmptyStr) {
                ss << *it;
            }
        }

        if (it + 1 != src.cend()) {
            ss << delimiter;
        }
    }
    return ss.str();
}

bool IsEqual(const std::string& s1, const std::string& s2, bool ignoreCase) {
    const std::string::size_type s1_len = s1.length();
    if (s1_len != s2.length())
        return false;

    for (std::string::size_type i = 0; i < s1_len; i++) {
        if (ignoreCase) {
            if (ToLower(s1[i]) != ToLower(s2[i]))
                return false;
        }
        else {
            if (s1[i] != s2[i])
                return false;
        }
    }

    return true;
}

bool IsEqual(const std::wstring& s1, const std::wstring& s2, bool ignoreCase) {
    const std::wstring::size_type s1_len = s1.length();
    if (s1_len != s2.length())
        return false;

    for (std::wstring::size_type i = 0; i < s1_len; i++) {
        if (ignoreCase) {
            if (ToLower(s1[i]) != ToLower(s2[i]))
                return false;
        }
        else {
            if (s1[i] != s2[i])
                return false;
        }
    }

    return true;
}

// format a string
bool StrFormat(const char* format, va_list argList, std::string& output) noexcept {
    if (!format)
        return false;

    bool ret = false;
#ifdef ASHE_WIN
    char* pMsgBuffer = NULL;
    size_t iMsgBufCount = 0;

    HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    try {
        while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
            iMsgBufCount += 1024;
            if (pMsgBuffer) {
                free(pMsgBuffer);
                pMsgBuffer = NULL;
            }

            pMsgBuffer = (char*)malloc(iMsgBufCount * sizeof(char));
            if (!pMsgBuffer) {
                break;
            }
            hr = StringCchVPrintfA(pMsgBuffer, iMsgBufCount, format, argList);
        }

        if (hr == S_OK && pMsgBuffer) {
            output.assign(pMsgBuffer);
        }

        if (pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }

        ret = (hr == S_OK);
    } catch (std::exception& e) {
        // Can't use CheckFailure macro.
        ASHE_UNUSED(e);
        ret = false;
    }

    return ret;
#else
    char* msgBuf = nullptr;
    size_t msgBufSize = 1024;

    try {
        do {
            if (msgBuf) {
                free(msgBuf);
                msgBuf = nullptr;
            }
            msgBuf = (char*)malloc(msgBufSize * sizeof(char));
            if (!msgBuf) {
                break;
            }
            memset(msgBuf, 0, msgBufSize * sizeof(char));

            va_list va_copy;
            VA_COPY(va_copy, argList);
            const int err = vsnprintf(msgBuf, msgBufSize, format, va_copy);
            if (err >= 0 && err < msgBufSize) {
                ret = true;
                break;
            }

            msgBufSize *= 2;
        } while (true);

        if (ret && msgBuf) {
            output.assign(msgBuf);
        }

        if (msgBuf) {
            free(msgBuf);
            msgBuf = nullptr;
        }
    } catch (std::exception& e) {
        // Can't use CheckFailure macro.
        ASHE_UNUSED(e);
        ret = false;
    }

    return ret;
#endif
}

bool StrFormat(const wchar_t* format, va_list argList, std::wstring& output) noexcept {
    if (!format)
        return false;

    bool ret = false;
#ifdef ASHE_WIN
    wchar_t* pMsgBuffer = NULL;
    size_t iMsgBufCount = 0;

    HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;
    try {
        while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
            iMsgBufCount += 1024;
            if (pMsgBuffer) {
                free(pMsgBuffer);
                pMsgBuffer = NULL;
            }

            pMsgBuffer = (wchar_t*)malloc(iMsgBufCount * sizeof(wchar_t));
            if (!pMsgBuffer) {
                break;
            }
            hr = StringCchVPrintfW(pMsgBuffer, iMsgBufCount, format, argList);
        }

        if (hr == S_OK && pMsgBuffer) {
            output.assign(pMsgBuffer);
        }

        if (pMsgBuffer) {
            free(pMsgBuffer);
            pMsgBuffer = NULL;
        }

        ret = (hr == S_OK);
    } catch (std::exception& e) {
        ASHE_UNUSED(e);
        ret = false;
    }

    return ret;
#else
    wchar_t* msgBuf = nullptr;
    size_t msgBufSize = 1024;

    try {
        do {
            if (msgBuf) {
                free(msgBuf);
                msgBuf = nullptr;
            }
            msgBuf = (wchar_t*)malloc(msgBufSize * sizeof(wchar_t));
            if (!msgBuf) {
                break;
            }
            memset(msgBuf, 0, msgBufSize * sizeof(wchar_t));

            va_list va_copy;
            VA_COPY(va_copy, argList);
            const int err = vswprintf(msgBuf, msgBufSize, format, va_copy);
            if (err >= 0 && err < msgBufSize) {
                ret = true;
                break;
            }

            msgBufSize *= 2;
        } while (true);

        if (ret && msgBuf) {
            output.assign(msgBuf);
        }

        if (msgBuf) {
            free(msgBuf);
            msgBuf = nullptr;
        }
    } catch (std::exception& e) {
        ASHE_UNUSED(e);
        ret = false;
    }

    return ret;
#endif
}

std::string StrFormat(const char* format, ...) noexcept {
    std::string output;
    try {
        va_list args;
        va_start(args, format);

        StrFormat(format, args, output);
        va_end(args);
    } catch (std::exception& e) {
        // Can't use CheckFailure macro.
        ASHE_UNUSED(e);
        output.clear();
    }

    return output;
}

std::wstring StrFormat(const wchar_t* format, ...) noexcept {
    std::wstring output;
    try {
        va_list args;
        va_start(args, format);
        StrFormat(format, args, output);
        va_end(args);
    } catch (std::exception& e) {
        // Can't use CheckFailure macro.
        ASHE_UNUSED(e);
        output.clear();
    }
    return output;
}

std::string StrFormat(const char* format, va_list argList) noexcept {
    std::string output;
    StrFormat(format, argList, output);
    return output;
}

std::wstring StrFormat(const wchar_t* format, va_list argList) noexcept {
    std::wstring output;
    StrFormat(format, argList, output);
    return output;
}

#ifdef ASHE_WIN
bool IsResourceString(const std::wstring& s) {
    std::wstring s2 = StrTrim(s, L" \"");
    return IsStartsWith(s2, L"@");
}

bool LoadStringFromRes(const std::wstring& resStr, std::wstring& result) {
    std::wstring resStrFormat = StrTrim(resStr, L" \"");
    resStrFormat = resStrFormat.substr(1);  // @

    if (resStrFormat.empty())
        return false;

    std::vector<std::wstring> v = StrSplit(resStrFormat, L",", true);
    if (v.size() < 2)
        return false;

    if (v[0].empty() || v[1].empty())
        return false;

    std::wstring envExpanded = ExpandEnvString(v[0]);
    DWORD dwBinType = 0;
    if (GetBinaryTypeW(envExpanded.c_str(), &dwBinType)) {
#ifdef ASHE_WIN32
        if (dwBinType == SCS_64BIT_BINARY) {
            return false;
        }
#endif
    }

    HMODULE hDll = LoadLibraryW(envExpanded.c_str());
    if (!hDll) {
        return false;
    }

    if (IsStartsWith(v[1], L"-"))
        v[1] = v[1].substr(1);

    UINT id = _wtoi(v[1].c_str());

    wchar_t szBuf[MAX_PATH + 1] = {0};
    int numberOfBytes = LoadStringW(hDll, id, szBuf, MAX_PATH);
    if (numberOfBytes <= 0) {
        FreeLibrary(hDll);
        return false;
    }

    result = szBuf;
    FreeLibrary(hDll);

    return true;
}

std::wstring Win32ErrCodeToStr(LONG err) {
    std::wstring result;
    LPWSTR buffer = NULL;
    DWORD dwRet = FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPWSTR)&buffer,
        NULL,
        NULL);

    if (dwRet > 0) {
        if (buffer) {
            result = buffer;
            LocalFree(buffer);
        }
    }

    return result;
}

#endif
}  // namespace ashe