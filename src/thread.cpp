#include "ashe/config.h"
#include "ashe/thread.hpp"
#if defined ASHE_WIN || defined ASHE_LINUX
#ifdef ASHE_WIN
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#elif (defined ASHE_LINUX)
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/syscall.h>
#endif

namespace ashe {
Thread::Thread() :
    thread_id_(0), exit_(false) {
    running_.store(false);
}

Thread::Thread(const std::string& name) :
    thread_id_(0), exit_(false), thread_name_(name) {
    running_.store(false);
}

Thread::~Thread() {
    stop(false);
}

void Thread::setName(const std::string& name) {
    thread_name_ = name;
}

std::string Thread::name() const {
    return thread_name_;
}

long Thread::id() {
    return thread_id_;
}

bool Thread::start() {
    if (thread_.valid()) {
        if (thread_.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
            return false;
        }
    }
    thread_ = std::async(std::launch::async, &Thread::run, this);
    return true;
}

void Thread::stop(bool waitUntilAllTasksFinish) {
    if (waitUntilAllTasksFinish) {
        do {
            {
                std::unique_lock<std::mutex> lg(mutex_);
                if (work_queue_.empty())
                    break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(60));
        } while (true);
    }

    {
        std::lock_guard<std::mutex> lg(mutex_);
        exit_ = true;
        exit_cond_var_.notify_one();
    }

    if (thread_.valid())
        thread_.wait();
}

bool Thread::isRunning() const {
    return running_.load();
}

void Thread::run() {
    running_.store(true);

    SetCurrentThreadName(thread_name_.c_str());
    thread_id_ = Thread::GetCurThreadId();
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lg(mutex_);
            exit_cond_var_.wait(lg, [this]() { return (exit_ || !work_queue_.empty()); });
            if (exit_) {
                running_.store(false);
                return;
            }
            task = std::move(work_queue_.front());
            work_queue_.pop();
        }

        task();
    }
}

void Thread::SetCurrentThreadName(const char* name) {
#ifdef ASHE_WIN
    struct {
        DWORD dwType;
        LPCSTR szName;
        DWORD dwThreadID;
        DWORD dwFlags;
    } threadname_info = {0x1000, name, static_cast<DWORD>(-1), 0};

    __try {
        ::RaiseException(0x406D1388, 0, sizeof(threadname_info) / sizeof(DWORD),
                         reinterpret_cast<ULONG_PTR*>(&threadname_info));
    } __except (EXCEPTION_EXECUTE_HANDLER) {  // NOLINT
    }
#elif defined ASHE_LINUX
    prctl(PR_SET_NAME, reinterpret_cast<unsigned long>(name));  // NOLINT
#endif
}

long Thread::GetCurThreadId() {
#ifdef ASHE_WIN
    return GetCurrentThreadId();
#elif defined ASHE_LINUX
    return static_cast<long>(syscall(__NR_gettid));
#endif
}
}  // namespace ashe
#endif