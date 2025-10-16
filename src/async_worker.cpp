#include "ashe/config.h"
#include "ashe/async_worker.h"
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
#include <thread>

namespace ashe {
AsyncWorker::AsyncWorker() :
    exit_(false) {
    running_.store(false);
}

AsyncWorker::~AsyncWorker() {
    stop();
}

bool AsyncWorker::start() {
    try {
        if (thread_.valid()) {
            if (thread_.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout) {
                return true;
            }
        }
        thread_ = std::async(std::launch::async, &AsyncWorker::run, this);
        return true;
    } catch (...) {
        return false;
    }
}

void AsyncWorker::stop() {
    {
        std::lock_guard<std::mutex> lg(mutex_);
        std::queue<std::function<void()>> empty_queue;
        work_queue_.swap(empty_queue);
        exit_ = true;
        exit_cond_var_.notify_one();
    }

    if (thread_.valid())
        thread_.wait();
}

bool AsyncWorker::isRunning() const {
    return running_.load();
}

void AsyncWorker::run() {
    running_.store(true);
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

        if (task) {
            task();
        }
    }
}

void AsyncWorker::clearTasks() {
    std::unique_lock<std::mutex> lg(mutex_);
    std::queue<std::function<void()>> empty_queue;
    work_queue_.swap(empty_queue);
}

size_t AsyncWorker::tasksCount() const {
    std::unique_lock<std::mutex> lg(mutex_);
    return work_queue_.size();
}
}  // namespace ashe
#endif