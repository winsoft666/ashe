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

#ifndef HTTP_UTIL_HPP_
#define HTTP_UTIL_HPP_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"

#include <string>
#include <vector>
#include <map>

namespace ashe {
using HttpHeaders = std::multimap<std::string, std::string>;

// HTTP Request Datagram
class ASHE_API HttpReqDatagram {
   public:
    enum class METHOD {
        GET = 0,
        POST,
        HEAD,
        DEL,  // DELETE conflict with winnt.h DELETE macro.
        PUT,
        PATCH,
        OPTIONS,
        TRACE,
        CONNECT
    };

    HttpReqDatagram() = default;
    ~HttpReqDatagram() = default;

    METHOD method() const;
    void setMethod(METHOD m);

    HttpHeaders headers() const;
    void setHeaders(const HttpHeaders& headers);

    std::string url() const;
    void setUrl(const std::string& url);

    const std::vector<unsigned char>& body() const;
    std::vector<unsigned char>& body();

    void setBody(const std::vector<unsigned char>& data);
    void setBody(const void* data, size_t dataSize);

    void setSaveFileHandle(FILE* f);
    FILE* saveFileHandle() const;

    void setSaveFilePath(const std::string& filePath);
    std::string saveFilePath() const;

   private:
    METHOD method_ = METHOD::GET;
    HttpHeaders headers_;
    std::vector<unsigned char> bodyData_;
    std::string url_;

    FILE* saveFileHandle_ = nullptr;
    std::string saveFilePath_;
};

// HTTP Response Datagram
class ASHE_API HttpRspDatagram {
   public:
    HttpRspDatagram() = default;
    ~HttpRspDatagram() = default;

    int code() const;
    void setCode(int code);

    HttpHeaders headers() const;
    void addHeader(const std::string& key, const std::string& value);

    const std::vector<unsigned char>& body() const;
    std::vector<unsigned char>& body();

    FILE* saveFileHandle() const;
    void setSaveFileHandle(FILE* f);

    void setSaveFilePath(const std::string& filePath);
    std::string saveFilePath() const;

   private:
    int code_ = 0;
    HttpHeaders headers_;
    std::vector<unsigned char> bodyData_;

    FILE* saveFile_ = nullptr;
    std::string saveFilePath_;
};

class ASHE_API HttpUtil {
   public:
    static bool IsHttps(const std::string& url);
};
}  // namespace ashe
#endif  // !HTTP_UTIL_HPP_
