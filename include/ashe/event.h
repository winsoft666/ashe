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

#ifndef ASHE_EVENT_HPP__
#define ASHE_EVENT_HPP__
#pragma once

#include "ashe/config.h"
#include <mutex>
#include <condition_variable>
#include "ashe/macros.h"

namespace ashe {
// 事件(Event)类
// 事件可以被置为有信号(set)或无信号(unset)，线程可以通过wait方法等待事件被设置为有信号
//
class ASHE_API Event {
   public:
    Event(bool isSet = false) noexcept;

    ~Event() = default;

    // 将事件设置为有信号
    //
    void set();

    // 将事件设置为无信号
    //
    void unset();

    // 检查事件是否被设置为有信号
    //
    bool isSet() const;

    // 等待事件被设置为有信号
    // millseconds参数指定等待的最长时间（毫秒），默认值-1表示无限等待
    // 
    // 如果事件在指定时间内被设置为有信号，则返回true，否则返回false
    //
    bool wait(int64_t millseconds = -1);

   protected:
    bool is_set_ = false;
    mutable std::mutex set_lock_;
    std::condition_variable setted_event_;

    ASHE_DISALLOW_COPY_AND_MOVE(Event);
};
}  // namespace ashe
#endif  //!ASHE_EVENT_HPP__
