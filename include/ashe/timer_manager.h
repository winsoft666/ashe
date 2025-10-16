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

#ifndef ASHE_TIMER_MANAGER_HPP__
#define ASHE_TIMER_MANAGER_HPP__
#pragma once

#include "ashe/config.h"
#include <functional>
#include <chrono>
#include <memory>
#include "ashe/macros.h"

namespace ashe {

// 跨平台定时器
// 
// 在Windows系统上，定时器存在 10 毫秒的精度损失，Windows平台高精度定时器可使用 win/timer.h
//
class ASHE_API TimerManager {
   public:
    using TimerId = std::size_t;
    using TimerHandler = std::function<void(TimerId)>;
    using SteadyClock = std::chrono::time_point<std::chrono::steady_clock>;

    TimerManager() noexcept;
    ~TimerManager();

    // 初始化定时器管理器
    //
    void init();

    // 销毁定时器管理器
    // 
    // 可能会阻塞，需要等待当前正在执行的定时器hanlder结束
    //
    void destory();

    // 创建一个新的定时器
    // 定时器将在when时间第一次执行，以后每 period 周期执行一次
    // 默认 period 为 0，表示定时器只执行一次
    // 
    // 不要阻塞handler回调函数，否则可能会影响其他定时器，建议结合 ashe/thread.h 或 ashe/async_worker.h 将任务投递到独立线程中执行
    //
    // 返回定时器Id，可以使用 remove 方法来清除定时器
    //
    TimerId add(const SteadyClock& when, const TimerHandler& handler, const std::chrono::microseconds& period = std::chrono::microseconds::zero());

    // 创建一个新的定时器
    // 定时器将在 when 周期后第一次执行，以后每 period 周期执行一次
    // 默认 period 为 0，表示定时器只执行一次
    // 
    // 不要阻塞handler回调函数，否则可能会影响其他定时器，建议结合 ashe/thread.h 或 ashe/async_worker.h 将任务投递到独立线程中执行
    //
    // 返回定时器Id，可以使用 remove 方法来清除定时器
    //
    template <class Rep, class Period>
    inline TimerId add(const std::chrono::duration<Rep, Period>& when, const TimerHandler& handler, const std::chrono::microseconds& period = std::chrono::microseconds::zero()) {
        return add(std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::microseconds>(when), handler, period);
    }

    // 创建一个新的定时器
    // 定时器将在 afterMicroseconds 微秒后第一次执行，以后每 periodMicroseconds 周期执行一次
    // 默认 period 为 0，表示定时器只执行一次
    // 
    // 不要阻塞handler回调函数，否则可能会影响其他定时器，建议结合 ashe/thread.h 或 ashe/async_worker.h 将任务投递到独立线程中执行
    //
    // 返回定时器Id，可以使用 remove 方法来清除定时器
    //
    TimerId add(const uint64_t afterMicroseconds, const TimerHandler& handler, const uint64_t periodMicroseconds = 0);

    // 清除定时器
    // 
    // 定时器Id不存在则返回false
    //
    bool remove(TimerId id);

   protected:
    class Private;
    std::unique_ptr<Private> p_;

   private:
    void run();

    ASHE_DISALLOW_COPY_AND_MOVE(TimerManager);
};
}  // namespace ashe
#endif
