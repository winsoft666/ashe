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

#ifndef ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DEFAULT_H_
#define ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DEFAULT_H_
#pragma once

#include "ashe/config.h"
#include "ashe/macros.h"
#include <condition_variable>
#include <mutex>
#include "ashe/message_loop/message_pump.h"

namespace ashe {
class ASHE_API MessagePumpDefault final : public MessagePump {
   public:
    MessagePumpDefault() = default;
    ~MessagePumpDefault() final = default;

    // MessagePump methods:
    void run(Delegate* delegate) final;
    void quit() final;
    void scheduleWork() final;
    void scheduleDelayedWork(const TimePoint& delayed_work_time) final;

   private:
    // This flag is set to false when run() should return.
    bool keepRunning_ = true;

    // Used to sleep until there is more work to do.
    std::condition_variable event_;

    bool have_work_ = false;
    std::mutex haveWorkLock_;

    // The time at which we should call doDelayedWork.
    TimePoint delayedWorkTime_;

    ASHE_DISALLOW_COPY_AND_ASSIGN(MessagePumpDefault);
};

}  // namespace ashe

#endif  // ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DEFAULT_H_
