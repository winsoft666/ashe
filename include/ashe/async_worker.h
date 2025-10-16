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

#ifndef ASHE_ASYNC_WORKER_H__
#define ASHE_ASYNC_WORKER_H__
#pragma once

#include "ashe/arch.h"
#if defined ASHE_WIN || defined ASHE_LINUX
#include "ashe/config.h"
#include "ashe/macros.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include "ashe/compiler_specific.h"
#if ASHE_CPP_STANDARD_VER >= 201703L  // >= C++ 17
#include <type_traits>
#endif  // ASHE_CPP_STANDARD_VER

namespace ashe {
// 异步工作线程类
// 通过invoke方法将任务投递到工作线程，任务以先进先出的顺序执行
// invoke方法返回一个std::future对象，可用于获取任务的返回值
// 如果工作线程未启动，则invoke方法返回的future对象将处于未决状态，直到工作线程启动并执行任务
//
class ASHE_API AsyncWorker {
   public:
    ASHE_DISALLOW_COPY_AND_MOVE(AsyncWorker);

    AsyncWorker();

    // 会自动调用stop()
    //
    virtual ~AsyncWorker();

    virtual bool start();

    // 清除未执行的任务，并等待当前正在执行的任务，待执行完成后退出工作线程
    //
    virtual void stop();

    bool isRunning() const;

    virtual void run();

    // 线程安全的
    //
    void clearTasks();

    // 线程安全的
    //
    size_t tasksCount() const;

    // 线程安全的
    //
    template <class F, class... Args>
    auto invoke(F&& f, Args&&... args)
#if ASHE_CPP_STANDARD_VER >= 201703L  // >= C++ 17
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;
#else
        -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;
#endif
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

   protected:
    std::future<void> thread_;
    mutable std::mutex mutex_;
    std::condition_variable exit_cond_var_;
    bool exit_;
    std::queue<std::function<void()>> work_queue_;
    std::atomic_bool running_;
};
}  // namespace ashe
#endif
#endif  // !ASHE_ASYNC_WORKER_H__
