#include "ashe/config.hpp"
#include "ashe/singleton_process.hpp"

#ifdef ASHE_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <windows.h>
#include <strsafe.h>
#include "ashe/string_encode.hpp"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#endif
#include <assert.h>
#include "ashe/string_helper.hpp"

namespace ashe {
void SingletonProcess::markAndCheckStartup(const std::string& uniqueName) {
    if (uniqueName_.empty() && !uniqueName.empty()) {
        uniqueName_ = uniqueName;
        check();
    }
}

const std::string& SingletonProcess::uniqueName() const {
    return uniqueName_;
}

bool SingletonProcess::isPrimary() const {
    assert(!uniqueName_.empty());
    return isPrimary_;
}

void SingletonProcess::check() {
#ifdef ASHE_WIN
    const int kSizeOfMap = 1024 * sizeof(wchar_t);
    std::string secondaryStartupEventName, memFileName;
    if (StringHelper::IsStartsWith(uniqueName_, "Global\\")) {
        mutex_ = CreateEventA(NULL, TRUE, FALSE, uniqueName_.c_str());
        secondaryStartupEventName = uniqueName_ + "_secondaryStartupEventName";
    }
    else {
        mutex_ = CreateEventA(NULL, TRUE, FALSE, ("Global\\" + uniqueName_).c_str());
        secondaryStartupEventName = "Global\\" + uniqueName_ + "_secondaryStartupEventName";
    }
    memFileName = uniqueName_ + "_memFileName";

    const DWORD gle = GetLastError();
    isPrimary_ = true;

    if (mutex_) {
        if (gle == ERROR_ALREADY_EXISTS) {
            isPrimary_ = false;
        }
    }
    else {
        if (gle == ERROR_ACCESS_DENIED)
            isPrimary_ = false;
    }

    if (isPrimary_) {
        secondaryStartupEvent_ = CreateEventA(NULL, FALSE, FALSE, secondaryStartupEventName.c_str());
        assert(secondaryStartupEvent_);

        fileMapping_ = CreateFileMappingA(INVALID_HANDLE_VALUE,
                                          NULL,
                                          PAGE_READWRITE,
                                          0,
                                          kSizeOfMap,
                                          memFileName.c_str());
        assert(fileMapping_);
        if (fileMapping_) {
            viewOfFile_ = MapViewOfFile(fileMapping_, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
        }

        asyncQuery_ = std::async(std::launch::async, [kSizeOfMap, this]() {
            while (!exit_.isSet()) {
                if (WaitForSingleObject(secondaryStartupEvent_, 100) == WAIT_OBJECT_0) {
                    if (exit_.isSet())  // check again, secondaryStartupEvent_ will be set when exit
                        break;

                    if (cb_) {
                        std::string secondaryCML;

                        if (viewOfFile_) {
                            secondaryCML = (const char*)viewOfFile_;
                            memset(viewOfFile_, 0, kSizeOfMap);
                        }
                        cb_(secondaryCML);
                    }
                }
            }
        });
    }
    else {
        secondaryStartupEvent_ = OpenEventA(EVENT_MODIFY_STATE, FALSE, secondaryStartupEventName.c_str());
        assert(secondaryStartupEvent_);

        if (secondaryStartupEvent_) {
            fileMapping_ = OpenFileMappingA(FILE_MAP_WRITE, FALSE, memFileName.c_str());
            assert(fileMapping_);

            if (fileMapping_) {
                viewOfFile_ = MapViewOfFile(fileMapping_, FILE_MAP_WRITE, 0, 0, 0);
                assert(viewOfFile_);

                if (viewOfFile_) {
                    LPWSTR pCML = ::GetCommandLineW();
                    if (pCML) {
                        std::string strCMLu8 = ashe::StringEncode::UnicodeToUtf8(std::wstring(pCML));
                        memset(viewOfFile_, 0, kSizeOfMap);
                        StringCchCopyA((LPSTR)viewOfFile_, kSizeOfMap, strCMLu8.c_str());
                    }
                }
            }
            SetEvent(secondaryStartupEvent_);
        }
    }
#else
    pidFile_ = open(("/tmp/" + uniqueName_ + ".pid").c_str(), O_CREAT | O_RDWR, 0666);
    int rc = flock(pidFile_, LOCK_EX | LOCK_NB);
    if (rc) {
        if (EWOULDBLOCK == errno)
            isPrimary_ = false;
    }
    isPrimary_ = true;
#endif
}

SingletonProcess::~SingletonProcess() {
#ifdef ASHE_WIN
    if (mutex_) {
        CloseHandle(mutex_);
        mutex_ = NULL;
    }

    if (viewOfFile_) {
        UnmapViewOfFile(viewOfFile_);
    }

    if (fileMapping_) {
        CloseHandle(fileMapping_);
    }

    exit_.set();
    if (secondaryStartupEvent_) {
        SetEvent(secondaryStartupEvent_);  // for fast exit
    }

    if (asyncQuery_.valid())
        asyncQuery_.wait();

    if (secondaryStartupEvent_) {
        CloseHandle(secondaryStartupEvent_);
        secondaryStartupEvent_ = NULL;
    }

#else
    if (pidFile_ != -1) {
        close(pidFile_);
        pidFile_ = -1;
    }
#endif
}

#ifdef ASHE_WIN
void* SingletonProcess::mutex() const {
    return mutex_;
}
#else
int SingletonProcess::pidFile() const {
    return pidFile_;
}
#endif
}  // namespace ashe