#include "ashe/config.hpp"
#include "ashe/win_http.hpp"
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
#include <winhttp.h>
#include <Intsafe.h>
#pragma comment(lib, "winhttp")
#include "ashe/string_helper.hpp"
#include "ashe/file.hpp"

namespace ashe {
WinHttp::~WinHttp() {
    closeRequest();
    closeConnection();
    closeSession();
}

bool WinHttp::openSession(
    const std::wstring& userAgent,
    unsigned int dwAccessType,
    const std::wstring& proxyName,
    const std::wstring& proxyBypass,
    unsigned int dwFlags) {
    if (dwAccessType == WINHTTP_ACCESS_TYPE_NO_PROXY) {
        hSession_ = WinHttpOpen(userAgent.c_str(),
                                dwAccessType,
                                WINHTTP_NO_PROXY_NAME,
                                WINHTTP_NO_PROXY_BYPASS,
                                dwFlags);
    }
    else {
        hSession_ = WinHttpOpen(userAgent.c_str(),
                                dwAccessType,
                                proxyName.c_str(),
                                proxyBypass.c_str(),
                                dwFlags);
    }

    dwGLE_ = hSession_ ? 0 : GetLastError();

    return !!hSession_;
}

bool WinHttp::openConnect(const std::wstring& url) {
    if (hSession_) {
        if (WinHttpCheckPlatform()) {
            URL_COMPONENTS urlComp;
            ZeroMemory(&urlComp, sizeof(urlComp));
            urlComp.dwStructSize = sizeof(urlComp);

            WCHAR wszScheme[64] = {0};
            urlComp.lpszScheme = wszScheme;
            urlComp.dwSchemeLength = ARRAYSIZE(wszScheme);

            WCHAR wszHostName[1024] = {0};
            urlComp.lpszHostName = wszHostName;
            urlComp.dwHostNameLength = ARRAYSIZE(wszHostName);

            WCHAR wszUrlPath[1024] = {0};
            urlComp.lpszUrlPath = wszUrlPath;
            urlComp.dwUrlPathLength = ARRAYSIZE(wszUrlPath);

            WCHAR wszExtraInfo[1024] = {0};
            urlComp.lpszExtraInfo = wszExtraInfo;
            urlComp.dwExtraInfoLength = ARRAYSIZE(wszExtraInfo);

            if (WinHttpCrackUrl(url.c_str(), (DWORD)url.length(), ICU_ESCAPE, &urlComp)) {
                strHostName_ = urlComp.lpszHostName;
                strUrlPath_ = urlComp.lpszUrlPath;
                strExtraInfo_ = urlComp.lpszExtraInfo;
                nPort_ = urlComp.nPort;
                m_nScheme = urlComp.nScheme;
                hConnect_ = WinHttpConnect(hSession_, strHostName_.c_str(), nPort_, 0);

                dwGLE_ = hConnect_ ? 0 : GetLastError();
            }
            else {
                dwGLE_ = GetLastError();
            }
        }
    }

    return !!hConnect_;
}

bool WinHttp::openRequest(const std::wstring& method) {
    if (hConnect_) {
        const DWORD dwFlags = (m_nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0);
        hRequest_ = WinHttpOpenRequest(hConnect_,
                                       method.c_str(),
                                       (strUrlPath_ + strExtraInfo_).c_str(),
                                       NULL,
                                       WINHTTP_NO_REFERER,
                                       WINHTTP_DEFAULT_ACCEPT_TYPES,
                                       dwFlags);

        dwGLE_ = hRequest_ ? 0 : GetLastError();
    }

    return !!hRequest_;
}

bool WinHttp::sendRequest(void* lpBuffer, size_t bufSize) {
    DWORD dwBufSize = 0;
    if (SIZETToDWord(bufSize, &dwBufSize) != S_OK)
        return false;

    BOOL bResult = FALSE;
    if (hRequest_) {
        if (lpBuffer != NULL && dwBufSize > 0)
            bResult = WinHttpSendRequest(hRequest_, WINHTTP_NO_ADDITIONAL_HEADERS, 0, lpBuffer, dwBufSize, dwBufSize, 0);
        else
            bResult = WinHttpSendRequest(hRequest_, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

        dwGLE_ = bResult ? 0 : GetLastError();
    }

    return !!bResult;
}

bool WinHttp::receiveResponse() {
    BOOL bResult = FALSE;
    if (hRequest_) {
        bResult = WinHttpReceiveResponse(hRequest_, NULL);

        dwGLE_ = bResult ? 0 : GetLastError();
    }
    return !!bResult;
}

bool WinHttp::getResponseBody(std::vector<unsigned char>& data) {
    bool result = true;
    DWORD dwSize = 0;
    if (!hRequest_)
        return false;

    do {
        // Check for available data.
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest_, &dwSize)) {
            result = false;
            dwGLE_ = GetLastError();
            break;
        }

        // No more available data.
        if (dwSize == 0)
            break;

        // Allocate space for the buffer.
        BYTE* lpReceivedData = new BYTE[dwSize];
        if (!lpReceivedData) {
            result = false;
            break;
        }

        // Read the Data.
        ZeroMemory(lpReceivedData, dwSize);

        DWORD dwRead = 0;
        if (!WinHttpReadData(hRequest_, lpReceivedData, dwSize, &dwRead)) {
            result = false;
            dwGLE_ = GetLastError();
            delete[] lpReceivedData;
            break;
        }

        // TODO: optimize
        for (size_t i = 0; i < dwSize; i++) {
            data.push_back(lpReceivedData[i]);
        }

        // Free the memory allocated to the buffer.
        delete[] lpReceivedData;
    } while (dwSize > 0);

    return result;
}

bool WinHttp::saveResponseBodyToFile(const std::wstring& filePath) {
    if (!hRequest_)
        return false;

    FILE* f = nullptr;
    _wfopen_s(&f, filePath.c_str(), L"wb");
    if (!f)
        return false;

    bool result = saveResponseBodyToFile(f);

    fclose(f);

    return result;
}

bool WinHttp::saveResponseBodyToFile(FILE* f) {
    if (!hRequest_)
        return false;

    if (!f)
        return false;

    bool result = true;
    DWORD dwSize = 0;
    do {
        // Check for available data.
        dwSize = 0;
        if (!WinHttpQueryDataAvailable(hRequest_, &dwSize)) {
            result = false;
            dwGLE_ = GetLastError();
            break;
        }

        // No more available data.
        if (dwSize == 0)
            break;

        // Allocate space for the buffer.
        BYTE* lpReceivedData = new BYTE[dwSize];
        if (!lpReceivedData) {
            result = false;
            break;
        }

        // Read the Data.
        ZeroMemory(lpReceivedData, dwSize);

        DWORD dwRead = 0;
        if (!WinHttpReadData(hRequest_, lpReceivedData, dwSize, &dwRead)) {
            dwGLE_ = GetLastError();
            delete[] lpReceivedData;
            result = false;
            break;
        }

        if (fwrite(lpReceivedData, 1, dwSize, f) != dwSize) {
            delete[] lpReceivedData;
            result = false;
            break;
        }

        // Free the memory allocated to the buffer.
        delete[] lpReceivedData;
    } while (dwSize > 0);

    return result;
}

unsigned int WinHttp::getResponseStatusCode() {
    DWORD dwSize = sizeof(DWORD);
    DWORD dwStatusCode = 0;
    std::wstring strStatus;

    BOOL bResults = WinHttpQueryHeaders(
        hRequest_,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &dwStatusCode,
        &dwSize,
        WINHTTP_NO_HEADER_INDEX);

    dwGLE_ = bResults ? 0 : GetLastError();

    return dwStatusCode;
}

std::wstring WinHttp::getResponseStatusText() {
    DWORD dwSize = 0;
    WCHAR* lpOutBuffer = NULL;
    std::wstring strStatus;

    BOOL bResults = WinHttpQueryHeaders(hRequest_, WINHTTP_QUERY_STATUS_TEXT, WINHTTP_HEADER_NAME_BY_INDEX, NULL, &dwSize, WINHTTP_NO_HEADER_INDEX);
    DWORD dwGLE = GetLastError();

    if (dwGLE == ERROR_INSUFFICIENT_BUFFER) {
        lpOutBuffer = new WCHAR[dwSize / sizeof(WCHAR)];
        bResults = WinHttpQueryHeaders(hRequest_, WINHTTP_QUERY_STATUS_TEXT, WINHTTP_HEADER_NAME_BY_INDEX, lpOutBuffer, &dwSize, WINHTTP_NO_HEADER_INDEX);

        if (bResults)
            strStatus.assign(lpOutBuffer, dwSize / sizeof(WCHAR));
        else
            dwGLE = GetLastError();
        delete[] lpOutBuffer;
    }

    dwGLE_ = dwGLE;

    return strStatus;
}

bool WinHttp::getResponseRawHeaders(std::wstring& headers) {
    DWORD dwSize = 0;
    WCHAR* lpOutBuffer = NULL;

    BOOL bResults = WinHttpQueryHeaders(hRequest_,
                                        WINHTTP_QUERY_RAW_HEADERS_CRLF,
                                        WINHTTP_HEADER_NAME_BY_INDEX,
                                        NULL,
                                        &dwSize,
                                        WINHTTP_NO_HEADER_INDEX);
    DWORD dwGLE = GetLastError();

    if (dwGLE == ERROR_INSUFFICIENT_BUFFER) {
        lpOutBuffer = new WCHAR[dwSize / sizeof(WCHAR)];
        bResults = WinHttpQueryHeaders(hRequest_,
                                       WINHTTP_QUERY_RAW_HEADERS_CRLF,
                                       WINHTTP_HEADER_NAME_BY_INDEX,
                                       lpOutBuffer,
                                       &dwSize,
                                       WINHTTP_NO_HEADER_INDEX);
        if (bResults)
            headers.assign(lpOutBuffer, dwSize / sizeof(WCHAR));
        else
            dwGLE = GetLastError();
        delete[] lpOutBuffer;
    }

    dwGLE_ = dwGLE;

    return bResults;
}

bool WinHttp::getResponseHeaders(std::unordered_map<std::wstring, std::wstring>& headerMap) {
    std::unordered_map<std::wstring, std::wstring> result;

    std::wstring strHeaders;

    if (!getResponseRawHeaders(strHeaders))
        return false;

    std::vector<std::wstring> headers = StringHelper::Split(strHeaders, L"\r\n", false);
    for (const auto& item : headers) {
        std::vector<std::wstring> v = StringHelper::Split(item, L": ", false);
        if (v.size() == 2) {
            headerMap[v[0]] = v[1];
        }
    }
    return true;
}

bool WinHttp::setRequestHeader(const std::wstring& name, const std::wstring& value) {
    if (name.empty() || value.empty())
        return false;

    BOOL bResults = FALSE;
    if (hRequest_) {
        const std::wstring strHeader(name + L": " + value);
        bResults = WinHttpAddRequestHeaders(hRequest_,
                                            strHeader.c_str(),
                                            (ULONG)-1L,
                                            WINHTTP_ADDREQ_FLAG_ADD_IF_NEW | WINHTTP_ADDREQ_FLAG_REPLACE);
        dwGLE_ = bResults ? 0 : GetLastError();
    }

    return !!bResults;
}

bool WinHttp::setTimeout(
    int nResolveTimeout,
    int nConnectTimeout,
    int nSendTimeout,
    int nReceiveTimeout) {
    BOOL bResults = FALSE;
    if (hSession_) {
        bResults = WinHttpSetTimeouts(hSession_,
                                      nResolveTimeout,
                                      nConnectTimeout,
                                      nSendTimeout,
                                      nReceiveTimeout);
        dwGLE_ = bResults ? 0 : GetLastError();
    }
    return bResults;
}

bool WinHttp::setOption(int Option, int value) {
    BOOL bResults = FALSE;
    if (hRequest_) {
        bResults = WinHttpSetOption(hRequest_, Option, (LPVOID)&value, 4);
        dwGLE_ = bResults ? 0 : GetLastError();
    }
    return bResults;
}

bool WinHttp::setIgnoreCAIssue(bool bIgnore) {
    DWORD dwFlags = 0;
    if (bIgnore)
        dwFlags = SECURITY_FLAG_IGNORE_UNKNOWN_CA |
                  SECURITY_FLAG_IGNORE_CERT_WRONG_USAGE |
                  SECURITY_FLAG_IGNORE_CERT_CN_INVALID |
                  SECURITY_FLAG_IGNORE_CERT_DATE_INVALID;

    return setOption(WINHTTP_OPTION_SECURITY_FLAGS, dwFlags);
}

bool WinHttp::setAllowRedirect(bool bAllow) {
    return setOption(WINHTTP_OPTION_REDIRECT_POLICY,
                     bAllow ? WINHTTP_OPTION_REDIRECT_POLICY_ALWAYS : WINHTTP_OPTION_REDIRECT_POLICY_NEVER);
}

void WinHttp::closeRequest() {
    if (hRequest_) {
        WinHttpCloseHandle(hRequest_);
        hRequest_ = NULL;
    }
}

void WinHttp::closeConnection() {
    if (hConnect_) {
        WinHttpCloseHandle(hConnect_);
        hConnect_ = NULL;
    }
}

void WinHttp::closeSession() {
    if (hSession_) {
        WinHttpCloseHandle(hSession_);
        hSession_ = NULL;
    }
}

unsigned long WinHttp::getErrorCode() const {
    return dwGLE_;
}

bool WinHttp::isOperationCancelled() const {
    return dwGLE_ == ERROR_WINHTTP_OPERATION_CANCELLED;
}

}  // namespace ashe
#endif