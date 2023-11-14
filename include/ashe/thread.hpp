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

#ifndef ASHE_THREAD_HPP__
#define ASHE_THREAD_HPP__
#pragma once

#include "ashe/arch.hpp"

#if defined ASHE_WIN || defined ASHE_LINUX
#include "ashe/config.hpp"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include "ashe/macros.hpp"

namespace ashe {
class ASHE_API Thread {
   public:
    ASHE_DISALLOW_COPY_MOVE(Thread);

    Thread();
    Thread(const std::string& name);

    virtual ~Thread();

    void setName(const std::string& name);
    std::string name() const;

    long id();

    virtual bool start();

    virtual void stop(bool waitUntilAllTasksFinish);

    bool isRunning() const;

    virtual void run();

    template <class F, class... Args>
    auto invoke(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();

        {
            std::unique_lock<std::mutex> lock(mutex_);
            work_queue_.emplace([task]() { (*task)(); });
        }
        exit_cond_var_.notify_one();
        return res;
    }

    static void SetCurrentThreadName(const char* name);

    static long GetCurThreadId();
   protected:
    std::string thread_name_;
    std::future<void> thread_;
    long thread_id_;
    std::mutex mutex_;
    std::condition_variable exit_cond_var_;
    bool exit_;
    std::queue<std::function<void()>> work_queue_;
    std::atomic_bool running_;
};
}  // namespace ashe
#endif
#endif  // !ASHE_THREAD_HPP__
