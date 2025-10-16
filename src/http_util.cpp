#include "ashe/config.h"
#include "ashe/http_util.h"
#include <string.h>
#include "ashe/string_util.h"

namespace ashe {
HttpReqDatagram::METHOD HttpReqDatagram::method() const {
    return method_;
}

void HttpReqDatagram::setMethod(METHOD m) {
    method_ = m;
}

HttpHeaders HttpReqDatagram::headers() const {
    return headers_;
}

void HttpReqDatagram::setHeaders(const HttpHeaders& headers) {
    headers_ = headers;
}

std::string HttpReqDatagram::url() const {
    return url_;
}

void HttpReqDatagram::setUrl(const std::string& url) {
    url_ = url;
}

const std::vector<unsigned char>& HttpReqDatagram::body() const {
    return bodyData_;
}

std::vector<unsigned char>& HttpReqDatagram::body() {
    return bodyData_;
}

void HttpReqDatagram::setBody(const std::vector<unsigned char>& data) {
    bodyData_ = data;
}

void HttpReqDatagram::setBody(const void* data, size_t dataSize) {
    bodyData_.resize(dataSize);
    memcpy(&bodyData_[0], data, dataSize);
}

void HttpReqDatagram::setSaveFileHandle(FILE* f) {
    saveFileHandle_ = f;
}

FILE* HttpReqDatagram::saveFileHandle() const {
    return saveFileHandle_;
}

void HttpReqDatagram::setSaveFilePath(const std::string& filePath) {
    saveFilePath_ = filePath;
}

std::string HttpReqDatagram::saveFilePath() const {
    return saveFilePath_;
}

//////////////////////////////////////////////////////////////////////////
// HttpResponseDatagram
//////////////////////////////////////////////////////////////////////////

int HttpRspDatagram::code() const {
    return code_;
}

void HttpRspDatagram::setCode(int code) {
    code_ = code;
}

HttpHeaders HttpRspDatagram::headers() const {
    return headers_;
}

void HttpRspDatagram::addHeader(const std::string& key,
                                           const std::string& value) {
    headers_.insert(std::make_pair(key, value));
}

const std::vector<unsigned char>& HttpRspDatagram::body() const {
    return bodyData_;
}

std::vector<unsigned char>& HttpRspDatagram::body() {
    return bodyData_;
}

FILE* HttpRspDatagram::saveFileHandle() const {
    return saveFile_;
}

void HttpRspDatagram::setSaveFileHandle(FILE* f) {
    saveFile_ = f;
}

void HttpRspDatagram::setSaveFilePath(const std::string& filePath) {
    saveFilePath_ = filePath;
}

std::string HttpRspDatagram::saveFilePath() const {
    return saveFilePath_;
}

bool IsHttpsUrl(const std::string& url) {
    if (ashe::StrToLower(url.substr(0, 6)) == "https:")
        return true;
    return false;
}

}  // namespace ashe