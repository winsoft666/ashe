/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 winsoft666 <winsoft666@outlook.com>.
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

#include "ashe/config.hpp"
#include <mutex>
#include <condition_variable>
#include "macros.hpp"
#include "ashe/variant.hpp"

namespace ashe {
class ASHE_API Event {
   public:
    using DataType = ashe::variant<bool, int, double, std::string, std::wstring>;
    ASHE_DISALLOW_COPY_MOVE(Event);
    Event(bool isSet = false);

    ~Event() = default;

    void set() noexcept;
    void set(const DataType& d) noexcept;

    void unset() noexcept;
    void unset(const DataType& d) noexcept;

    void cancel() noexcept;

    void unCancel() noexcept;

    // is equal unset() and unCancel().
    void reset() noexcept;
    void reset(const DataType& d) noexcept;

    bool isSet() noexcept;

    bool isCancelled() noexcept;

    // is infinite when millseconds < 0.
    bool wait(int64_t millseconds = -1) noexcept;
    bool wait(DataType &d, int64_t millseconds = -1) noexcept;

   protected:
    bool is_set_ = false;
    bool is_cancelld_ = false;
    DataType data_;
    std::mutex set_mutex_;
    std::condition_variable setted_cond_var_;
};
}  // namespace ashe
#endif  //!ASHE_EVENT_HPP__
