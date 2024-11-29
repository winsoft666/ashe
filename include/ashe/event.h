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
class ASHE_API Event {
   public:
    Event(bool isSet = false) noexcept;

    ~Event() = default;

    void set();
    void set(int64_t d);

    void unset();
    void unset(int64_t d);

    // equal unset()
    void reset();
    void reset(int64_t d);

    bool isSet() const;

    // infinite when millseconds < 0.
    bool wait(int64_t millseconds = -1);
    bool wait(int64_t& d, int64_t millseconds = -1);
   protected:
    bool is_set_ = false;
    int64_t data_ = 0;
    std::mutex set_lock_;
    std::condition_variable setted_event_;

    ASHE_DISALLOW_COPY_AND_MOVE(Event);
};
}  // namespace ashe
#endif  //!ASHE_EVENT_HPP__
