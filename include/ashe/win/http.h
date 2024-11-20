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

#ifndef ASHE_WIN_HTTP_HPP__
#define ASHE_WIN_HTTP_HPP__
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "ashe/macros.h"

namespace ashe {
namespace win {
// TODO: Use WinHTTP in asynchronous mode.
//
class ASHE_API Http {
   public:
    ASHE_DISALLOW_COPY_MOVE(Http);

    Http() = default;

    virtual ~Http();

    // dwAccessType defined in winhttp.h:
    //
    // #define WINHTTP_ACCESS_TYPE_DEFAULT_PROXY 0
    // #define WINHTTP_ACCESS_TYPE_NO_PROXY 1
    // #define WINHTTP_ACCESS_TYPE_NAMED_PROXY 3
    // #define WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY 4
    //
    // dwFlags:
    // WINHTTP_FLAG_ASYNC: Use the WinHTTP functions asynchronously.
    // WINHTTP_FLAG_SECURE_DEFAULTS: When this flag is set, Http will require use of TLS 1.2 or newer.
    //
    bool openSession(
        const std::wstring& userAgent = L"",
        unsigned int dwAccessType = 1,
        const std::wstring& proxyName = L"",
        const std::wstring& proxyBypass = L"",
        unsigned int dwFlags = 0);

    bool openConnect(const std::wstring& url);

    bool openRequest(const std::wstring& method = L"GET");

    bool sendRequest(void* lpBuffer = nullptr, size_t bufSize = 0);

    bool receiveResponse();

    bool getResponseBody(std::vector<unsigned char>& data);

    bool saveResponseBodyToFile(FILE* f);
    bool saveResponseBodyToFile(const std::wstring& filePath);

    unsigned int getResponseStatusCode();

    std::wstring getResponseStatusText();

    bool getResponseRawHeaders(std::wstring& headers);

    bool getResponseHeaders(std::unordered_map<std::wstring, std::wstring>& headerMap);

    bool setRequestHeader(const std::wstring& name, const std::wstring& value);

    bool setTimeout(
        int nResolveTimeout = 3000,
        int nConnectTimeout = 3000,
        int nSendTimeout = 3000,
        int nReceiveTimeout = 3000);

    // See: https://learn.microsoft.com/en-us/windows/win32/winhttp/option-flags
    bool setOption(int Option, int value);

    bool setIgnoreCAIssue(bool bIgnore);

    bool setAllowRedirect(bool bAllow);

    void closeRequest();

    void closeConnection();

    void closeSession();

    unsigned long getErrorCode() const;

    bool isOperationCancelled() const;

   protected:
    void* hSession_ = nullptr;  // HINTERNET
    void* hConnect_ = nullptr;  // HINTERNET
    void* hRequest_ = nullptr;

    std::wstring strHostName_;
    std::wstring strUrlPath_;
    std::wstring strExtraInfo_;
    int m_nScheme = 0;  // INTERNET_SCHEME
    unsigned int nPort_ = 0;

    unsigned long dwGLE_ = 0;
};
}  // namespace win
}  // namespace ashe
#endif
