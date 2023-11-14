/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022~2023 winsoft666 <winsoft666@outlook.com>.
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

#ifndef WIN_HTTP_CLIENT_HPP_
#define WIN_HTTP_CLIENT_HPP_
#pragma once

#include "ashe/config.hpp"
#include "ashe/arch.hpp"
#ifdef ASHE_WIN
#include <string>
#include <map>
#include <future>
#include "win_http.hpp"
#include "http_util.hpp"

namespace ashe {
// Http Client
class ASHE_API WinHttpClient {
   public:
    using RequestResult =
        std::function<void(long,                   // =0 is success, >0 is lasterror
                           unsigned long,          // microseconds
                           const HttpRspDatagram&  // HTTP response datagram
                           )>;
    // Custom error code
    enum : int { PARAM_ERROR = -1,
                 UNKNOWN_ERROR = -2,
                 USER_ABORT = 12017 /*ERROR_WINHTTP_OPERATION_CANCELLED*/ };

    WinHttpClient();
    WinHttpClient(const WinHttpClient&) = delete;
    WinHttpClient& operator=(const WinHttpClient&) = delete;
    virtual ~WinHttpClient();

   public:
    // Issue an asynchronous HTTP request.
    // Return value:
    //   The return value does not indicate that the request is finished.
    //   Return false only when has another request is doing, other case will return true.
    //
    bool request(std::shared_ptr<HttpReqDatagram> reqDatagram,
                 RequestResult ret,
                 bool allowRedirect = true,
                 int timeout = 5000  // ms
    );

    // Abort the current HTTP request.
    //
    void abort();

    // Wait for the current HTTP request to be finished.
    // Return value:
    //   true  - request has finished or request finished with the special time(ms).
    //   false - timeout, the request did not finished with the special time(ms).
    //
    bool wait(int ms);

    // Set HTTP proxy
    void setProxy(const std::string& proxy);
    std::string proxy() const;

    std::shared_ptr<WinHttp> winHttp();

   private:
    int doRequest(std::shared_ptr<HttpReqDatagram> reqDg,
                  HttpRspDatagram& rspDg,
                  bool allowRedirect,
                  int timeout);

    int checkErrorCode(unsigned long dwGLE);

   private:
    std::future<void> future_;
    std::atomic_bool abort_;
    std::string proxy_;
    std::shared_ptr<WinHttp> winHttp_;
};
}  // namespace ashe
#endif  // ASHE_WIN
#endif  // !WIN_HTTP_CLIENT_HPP_