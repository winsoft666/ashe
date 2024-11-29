#include "ashe/config.h"
#include "ashe/file.h"

#ifdef ASHE_WIN
#include <io.h>
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
#include <Shlwapi.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif  // !ASHE_WIN
#include "ashe/string_encode.h"
#include "ashe/logging.h"

namespace ashe {

File::File(const std::wstring& path) {
#ifdef ASHE_WIN
    path_ = path;
#else
    path_ = w2u(path);
#endif
}

File::File(const std::string& path) {
#ifdef ASHE_WIN
    path_ = a2w(path);
#else
    path_ = path;
#endif
}

File::~File() {
    close();
}

std::wstring File::pathW() const {
#ifdef ASHE_WIN
    return path_;
#else
    return u2w(path_);
#endif
}

std::string File::pathA() const {
#ifdef ASHE_WIN
    return w2a(path_);
#else
    return path_;
#endif
}

bool File::isOpen() {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    return (f_ != nullptr);
}

bool File::open(const std::wstring& openMode) {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (f_ != nullptr)
        return true;

    if (path_.empty() || openMode.empty())
        return false;

#ifdef ASHE_WIN
    _wfopen_s(&f_, path_.c_str(), openMode.c_str());
#else
    f_ = fopen(path_.c_str(), w2u(openMode).c_str());
#endif

    return (f_ != nullptr);
}

bool File::open(const std::string& openMode) {
    std::wstring openModeW;
#ifdef ASHE_WIN
    openModeW = AnsiToUnicode(openMode);
#else
    openModeW = Utf8ToUnicode(openMode);
#endif
    return open(openModeW);
}

bool File::close() {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (f_) {
        const int err = fclose(f_);
        if (err == 0)
            f_ = nullptr;
        return (err == 0);
    }
    return false;
}

bool File::flush() {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (f_)
        return (fflush(f_) == 0);
    return false;
}

bool File::isExist() const {
    if (path_.empty())
        return false;
#ifdef ASHE_WIN
    return (_waccess(path_.c_str(), 0) == 0);
#else
    return (access(path_.c_str(), F_OK) == 0);
#endif
}

bool File::canRW() const {
    if (path_.empty())
        return false;

#ifdef ASHE_WIN
    return (_waccess(path_.c_str(), 6) == 0);
#else
    return (access(path_.c_str(), R_OK | W_OK) == 0);
#endif
}

int64_t File::fileSize() {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (!f_)
        return -1;

#ifdef ASHE_WIN
    const int64_t curPos = _ftelli64(f_);
    if (curPos == -1L)
        return -1;

    if (_fseeki64(f_, 0L, SEEK_END) != 0)
        return -1;

    const int64_t ret = _ftelli64(f_);
    _fseeki64(f_, curPos, SEEK_SET);

    return ret;
#else
    const int64_t curPos = ftello64(f_);
    if (curPos == -1L)
        return -1;

    if (fseeko64(f_, 0L, SEEK_END) != 0)
        return -1;

    const int64_t ret = ftello64(f_);
    fseeko64(f_, curPos, SEEK_SET);

    return ret;
#endif
}

// Must be call open(...) first!
bool File::seekFromCurrent(int64_t offset) {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (f_) {
#ifdef ASHE_WIN
        return (_fseeki64(f_, offset, SEEK_CUR) == 0);
#else
        return (fseeko64(f_, offset, SEEK_CUR) == 0);
#endif
    }
    return false;
}

// Return: -1 is failed
int64_t File::currentPointerPos() {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (!f_)
        return -1;
#ifdef ASHE_WIN
    return _ftelli64(f_);
#else
    return ftello64(f_);
#endif
}

bool File::seekFromBeginning(int64_t offset) {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (f_) {
#ifdef ASHE_WIN
        return (_fseeki64(f_, offset, SEEK_SET) == 0);
#else
        return (fseeko64(f_, offset, SEEK_SET) == 0);
#endif
    }
    return false;
}

bool File::seekFromEnd(int64_t offset) {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (f_) {
#ifdef ASHE_WIN
        return (_fseeki64(f_, offset, SEEK_END) == 0);
#else
        return (fseeko64(f_, offset, SEEK_END) == 0);
#endif
    }
    return false;
}

size_t File::readFrom(void* buffer, size_t needRead, int64_t from) {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (!f_ || !buffer || needRead == 0)
        return 0;

    if (from >= 0) {
#ifdef ASHE_WIN
        if (_fseeki64(f_, from, SEEK_SET) != 0)
            return 0;
#else
        if (fseeko64(f_, from, SEEK_SET) != 0)
            return 0;
#endif
    }

    const size_t read = fread(buffer, 1, needRead, f_);
    return read;
}

size_t File::writeFrom(const void* buffer, size_t needWrite, int64_t from) {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (!f_ || !buffer || needWrite == 0)
        return 0;

    if (from >= 0) {
#ifdef ASHE_WIN
        if (_fseeki64(f_, from, SEEK_SET) != 0)
            return 0;
#else
        if (fseeko64(f_, from, SEEK_SET) != 0)
            return 0;
#endif
    }

    const size_t written = fwrite(buffer, 1, needWrite, f_);
    return written;
}

bool File::readAll(std::vector<uint8_t>& buffer) {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (!f_)
        return false;

#ifdef ASHE_WIN
    const int64_t curPos = _ftelli64(f_);
    if (curPos == -1L)
        return false;

    if (_fseeki64(f_, 0L, SEEK_END) != 0)
        return false;

    const int64_t fileSize = _ftelli64(f_);
    if (_fseeki64(f_, 0, SEEK_SET) != 0)
        return false;

    size_t read = 0;
    try {
        buffer.resize(fileSize);
    } catch (std::exception& e) {
        DLOG(LS_FATAL) << "exception occurred: " << e.what();
        return false;
    }
    read = fread(buffer.data(), 1, (size_t)fileSize, f_);

    _fseeki64(f_, curPos, SEEK_SET);

    return read == fileSize;
#else
    const int64_t curPos = ftello64(f_);
    if (curPos == -1L)
        return false;

    if (fseeko64(f_, 0L, SEEK_END) != 0)
        return false;

    const int64_t fileSize = ftello64(f_);
    if (fseeko64(f_, 0, SEEK_SET) != 0)
        return false;

    size_t read = 0;
    try {
        buffer.resize(fileSize);
    } catch (std::exception& e) {
        DLOG(LS_FATAL) << "exception occurred: " << e.what();
        return false;
    }
    read = fread(buffer.data(), 1, (size_t)fileSize, f_);

    fseeko64(f_, curPos, SEEK_SET);

    return read == fileSize;
#endif
}

std::string File::readAll() {
    std::vector<uint8_t> buffer;
    if (!readAll(buffer)) {
        return std::string();
    }

    std::string ret;
    ret.assign((const char* const)buffer.data(), buffer.size());
    return ret;
}

bool File::readAll(std::string& ret) {
    std::vector<uint8_t> buffer;
    if (!readAll(buffer)) {
        return false;
    }

    ret.clear();
    ret.assign((const char* const)buffer.data(), buffer.size());
    return true;
}
}  // namespace ashe