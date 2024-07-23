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
#ifdef __cpp_lib_variant
#include <variant>
#endif

namespace ashe {
class ASHE_API Event {
   public:
#ifdef __cpp_lib_variant
    using DataType = std::variant<bool, int, double, std::string, std::wstring>;
#endif
    ASHE_DISALLOW_COPY_MOVE(Event);
    Event(bool isSet = false) noexcept;

    ~Event() = default;

    void set();
#ifdef __cpp_lib_variant
    void set(const DataType& d);
#endif

    void unset();
#ifdef __cpp_lib_variant
    void unset(const DataType& d);
#endif

    void cancel();

    void unCancel();

    // is equal unset() and unCancel().
    void reset();
#ifdef __cpp_lib_variant
    void reset(const DataType& d);
#endif

    bool isSet();

    bool isCancelled();

    // is infinite when millseconds < 0.
    bool wait(int64_t millseconds = -1);
#ifdef __cpp_lib_variant
    bool wait(DataType& d, int64_t millseconds = -1);
#endif
   protected:
    bool is_set_ = false;
    bool is_cancelld_ = false;
#ifdef __cpp_lib_variant
    DataType data_;
#endif
    std::mutex set_mutex_;
    std::condition_variable setted_cond_var_;
};
}  // namespace ashe
#endif  //!ASHE_EVENT_HPP__
