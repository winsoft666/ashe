#include "ashe/config.hpp"
#include "ashe/file.hpp"

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

ashe::File::File(const fs::path& path) :
    path_(path) {
}

ashe::File::File(fs::path&& path) :
    path_(std::move(path)) {
}

ashe::File::~File() {
    close();
}

ashe::fs::path ashe::File::path() const {
    return path_;
}

bool ashe::File::isOpen() {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    return (f_ != nullptr);
}

bool ashe::File::open(const ashe::fs::path& openMode) {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (f_ != nullptr)
        return true;

    if (path_.empty() || openMode.empty())
        return false;

#ifdef ASHE_WIN
    _wfopen_s(&f_, path_.wstring().c_str(), openMode.wstring().c_str());
#else
    f_ = fopen(path_.u8string().c_str(), openMode.u8string().c_str());
#endif

    return (f_ != nullptr);
}

bool ashe::File::close() {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (f_) {
        const int err = fclose(f_);
        if (err == 0)
            f_ = nullptr;
        return (err == 0);
    }
    return false;
}

bool ashe::File::flush() {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (f_)
        return (fflush(f_) == 0);
    return false;
}

bool ashe::File::exist() const {
    if (path_.empty())
        return false;
#ifdef ASHE_WIN
    return (_waccess(path_.c_str(), 0) == 0);
#else
    return (access(path_.c_str(), F_OK) == 0);
#endif
}

bool ashe::File::canRW() const {
    if (path_.empty())
        return false;

#ifdef ASHE_WIN
    return (_waccess(path_.c_str(), 6) == 0);
#else
    return (access(path_.c_str(), R_OK | W_OK) == 0);
#endif
}

int64_t ashe::File::fileSize() {
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
bool ashe::File::seekFromCurrent(int64_t offset) {
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
int64_t ashe::File::currentPointerPos() {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (!f_)
        return -1;
#ifdef ASHE_WIN
    return _ftelli64(f_);
#else
    return ftello64(f_);
#endif
}

bool ashe::File::seekFromBeginning(int64_t offset) {
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

bool ashe::File::seekFromEnd(int64_t offset) {
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

size_t ashe::File::readFrom(void* buffer, size_t needRead, int64_t from) {
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

size_t ashe::File::writeFrom(const void* buffer, size_t needWrite, int64_t from) {
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

size_t ashe::File::readAll(void** buffer) {
    std::lock_guard<std::recursive_mutex> lg(mutex_);
    if (!f_ || !buffer)
        return 0;

#ifdef ASHE_WIN
    const int64_t curPos = _ftelli64(f_);
    if (curPos == -1L)
        return 0;

    if (_fseeki64(f_, 0L, SEEK_END) != 0)
        return 0;

    const int64_t fileSize = _ftelli64(f_);
    if (_fseeki64(f_, 0, SEEK_SET) != 0)
        return 0;

    size_t read = 0;
    if (fileSize <= SIZE_MAX) {
        *buffer = malloc((size_t)fileSize);
        if (*buffer != nullptr) {
            read = fread(*buffer, 1, (size_t)fileSize, f_);
        }
    }

    _fseeki64(f_, curPos, SEEK_SET);

    return read;
#else
    const int64_t curPos = ftello64(f_);
    if (curPos == -1L)
        return 0;

    if (fseeko64(f_, 0L, SEEK_END) != 0)
        return 0;

    const int64_t fileSize = ftello64(f_);
    if (fseeko64(f_, 0, SEEK_SET) != 0)
        return 0;

    size_t read = 0;
    if (fileSize <= SIZE_MAX) {
        *buffer = malloc((size_t)fileSize);
        if (*buffer != nullptr) {
            read = fread(*buffer, 1, (size_t)fileSize, f_);
        }
    }

    fseeko64(f_, curPos, SEEK_SET);

    return read;
#endif
}

std::string ashe::File::readAll() {
    std::string ret;
    void* buffer = nullptr;
    const size_t read = readAll(&buffer);

    if (buffer) {
        ret.assign((const char* const)buffer, read);
        free(buffer);
    }
    return ret;
}

bool ashe::File::readAll(std::string& ret) {
    void* buffer = nullptr;
    const size_t read = readAll(&buffer);

    if (buffer) {
        ret.assign((const char* const)buffer, read);
        free(buffer);
        return true;
    }

    return false;
}