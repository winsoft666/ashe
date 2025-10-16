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

#ifndef WIN_HTTP_CLIENT_HPP_
#define WIN_HTTP_CLIENT_HPP_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>
#include <map>
#include <future>
#include "ashe/win/http.h"
#include "ashe/http_util.h"

namespace ashe {
namespace win {
// 基于 WinHttp 接口实现的 HTTP 客户端，包含请求、响应、文件下载等
//
class ASHE_API HttpClient {
   public:
    using RequestResult =
        std::function<void(long,                   // 0 is success, > 0 is lasterror
                           unsigned long,          // microseconds
                           const HttpRspDatagram&  // HTTP response datagram
                           )>;
    // Custom error code
    enum : int {
        PARAM_ERROR = -1,
        UNKNOWN_ERROR = -2,
        USER_ABORT = 12017 // ERROR_WINHTTP_OPERATION_CANCELLED
    };

    HttpClient();
    virtual ~HttpClient();

   public:
    // 发起HTTP异步请求
    // 函数返回不代表请求完成，仅在当前HttpClient实例已有其他请求正在进行时才返回false，其他情况均返回true
    // 响应结果及内容通过 RequestResult 回调函数返回
    // 
    bool request(std::shared_ptr<HttpReqDatagram> reqDatagram,
                 RequestResult ret,
                 bool allowRedirect = true,
                 int timeout = 5000  // ms
    );

    // 取消/中止当前HTTP请求
    //
    void abort();

    // 等待当前的 HTTP 请求完成
    // 返回值：
    //   true  - 请求已完成或在指定时间（毫秒）内完成
    //   false - 超时，请求未在指定时间（毫秒）内完成

    bool wait(int ms);

    // 设置HTTP代理
    void setProxy(const std::string& proxy);
    std::string proxy() const;

    std::shared_ptr<Http> winHttp();

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
    std::shared_ptr<Http> winHttp_;

    ASHE_DISALLOW_COPY_AND_MOVE(HttpClient);
};
}  // namespace win
}  // namespace ashe
#endif  // !WIN_HTTP_CLIENT_HPP_