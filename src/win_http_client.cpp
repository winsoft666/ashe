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

#include "ashe/config.h"
#include "ashe/win_http_client.h"
#ifdef ASHE_WIN
#include <assert.h>
#include "ashe/windows_lite.h"
#include <winhttp.h>
#include "ashe/time_util.h"
#include "ashe/string_encode.h"

namespace ashe {
WinHttpClient::WinHttpClient(void) {
    winHttp_ = std::make_shared<WinHttp>();
    abort_.store(false);
}

WinHttpClient::~WinHttpClient(void) {
    abort();

    if (STD_ASYNC_IS_RUNNING(future_)) {
        future_.get();
    }

    if (winHttp_) {
        winHttp_.reset();
    }
}

bool WinHttpClient::request(std::shared_ptr<HttpReqDatagram> reqDatagram,
                         RequestResult reqRet,
                         bool allowRedirect /* = true*/,
                         int timeout /*= 5000*/) {
    if (!reqDatagram) {
        return false;
    }

    if (future_.valid() &&
        future_.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
        return false;
    }

    abort_.store(false);
    future_ = std::async(std::launch::async, [=]() {
        int64_t startTS = TimeUtil::GetCurrentTimestampByMilliSec();
        HttpRspDatagram rspDatagram;
        rspDatagram.setSaveFileHandle(reqDatagram->saveFileHandle());
        rspDatagram.setSaveFilePath(reqDatagram->saveFilePath());

        int ccode = doRequest(reqDatagram,
                              rspDatagram,
                              allowRedirect,
                              timeout);
        winHttp_->closeRequest();
        winHttp_->closeConnection();
        winHttp_->closeSession();

        if (reqRet) {
            int64_t endTS = TimeUtil::GetCurrentTimestampByMilliSec();
            reqRet(ccode, (unsigned long)(endTS - startTS), rspDatagram);
        }
    });

    return true;
}

int WinHttpClient::doRequest(std::shared_ptr<HttpReqDatagram> reqDatagram,
                          HttpRspDatagram& rspDatagram,
                          bool allowRedirect,
                          int timeout) {
    DWORD dwAccessType = WINHTTP_ACCESS_TYPE_NO_PROXY;
    if (!proxy_.empty())
        dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;

    if (!winHttp_->openSession(L"",
                               dwAccessType,
                               Utf8ToUnicode(proxy_),
                               L"",
                               0)) {
        return checkErrorCode(winHttp_->getErrorCode());
    }

    if (!winHttp_->setTimeout(timeout, timeout, timeout, timeout)) {
        return checkErrorCode(winHttp_->getErrorCode());
    }

    if (!winHttp_->openConnect(Utf8ToUnicode(reqDatagram->url()))) {
        return checkErrorCode(winHttp_->getErrorCode());
    }

    std::wstring strMethod;
    switch (reqDatagram->method()) {
        case HttpReqDatagram::METHOD::GET:
            strMethod = L"GET";
            break;
        case HttpReqDatagram::METHOD::POST:
            strMethod = L"POST";
            break;
        case HttpReqDatagram::METHOD::HEAD:
            strMethod = L"HEAD";
            break;
        case HttpReqDatagram::METHOD::CONNECT:
            strMethod = L"CONNECT";
            break;
        case HttpReqDatagram::METHOD::DEL:
            strMethod = L"DELETE";
            break;
        case HttpReqDatagram::METHOD::OPTIONS:
            strMethod = L"OPTIONS";
            break;
        case HttpReqDatagram::METHOD::PATCH:
            strMethod = L"PATCH";
            break;
        case HttpReqDatagram::METHOD::PUT:
            strMethod = L"PUT";
            break;
        default:
            assert(false);
    }

    if (!winHttp_->openRequest(strMethod)) {
        return checkErrorCode(winHttp_->getErrorCode());
    }

    if (!winHttp_->setAllowRedirect(allowRedirect)) {
        return checkErrorCode(winHttp_->getErrorCode());
    }

    const HttpHeaders& headers = reqDatagram->headers();
    for (const auto& it : headers) {
        if (!winHttp_->setRequestHeader(Utf8ToUnicode(it.first), Utf8ToUnicode(it.second))) {
            return checkErrorCode(winHttp_->getErrorCode());
        }
    }

    std::vector<unsigned char>& bodyData = reqDatagram->body();
    if (!winHttp_->sendRequest((void*)bodyData.data(), bodyData.size())) {
        return checkErrorCode(winHttp_->getErrorCode());
    }

    if (!winHttp_->receiveResponse()) {
        return checkErrorCode(winHttp_->getErrorCode());
    }

    std::unordered_map<std::wstring, std::wstring> headerMap;
    if (!winHttp_->getResponseHeaders(headerMap)) {
        return checkErrorCode(winHttp_->getErrorCode());
    }

    for (auto it = headerMap.begin(); it != headerMap.end(); it++) {
        rspDatagram.addHeader(UnicodeToUtf8(it->first), UnicodeToUtf8(it->second));
    }

    bool getRspRet = false;
    if (rspDatagram.saveFileHandle()) {
        getRspRet = winHttp_->saveResponseBodyToFile(rspDatagram.saveFileHandle());
    }
    else if (!rspDatagram.saveFilePath().empty()) {
        getRspRet = winHttp_->saveResponseBodyToFile(Utf8ToUnicode(rspDatagram.saveFilePath()));
    }
    else {
        getRspRet = winHttp_->getResponseBody(rspDatagram.body());
    }

    if (!getRspRet) {
        return checkErrorCode(winHttp_->getErrorCode());
    }

    return 0;
}

int WinHttpClient::checkErrorCode(unsigned long dwGLE) {
    int ret = UNKNOWN_ERROR;
    if (dwGLE == ERROR_WINHTTP_OPERATION_CANCELLED || abort_.load())
        ret = USER_ABORT;
    else if (dwGLE != 0)
        ret = dwGLE;

    return ret;
}

void WinHttpClient::abort() {
    abort_.store(true);

    if (winHttp_) {
        winHttp_->closeRequest();
        winHttp_->closeConnection();
        winHttp_->closeSession();
    }
}

bool WinHttpClient::wait(int ms) {
    if (!future_.valid())
        return true;

    if (ms < 0) {
        // 10 years
        return future_.wait_until(std::chrono::system_clock::now() +
                                  std::chrono::minutes(5256000)) ==
               std::future_status::ready;
    }
    return future_.wait_for(std::chrono::milliseconds(ms)) ==
           std::future_status::ready;
}

void WinHttpClient::setProxy(const std::string& proxy) {
    proxy_ = proxy;
}

std::string WinHttpClient::proxy() const {
    return proxy_;
}

std::shared_ptr<WinHttp> WinHttpClient::winHttp() {
    return winHttp_;
}

}  // namespace ashe

#endif  // ASHE_WIN