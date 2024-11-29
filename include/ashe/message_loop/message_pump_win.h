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

#ifndef ASHE_MESSAGE_LOOP_MESSAGE_PUMP_WIN_H_
#define ASHE_MESSAGE_LOOP_MESSAGE_PUMP_WIN_H_
#pragma once

#include "ashe/arch.h"
#ifdef ASHE_WIN
#include "ashe/config.h"
#include "ashe/message_loop/message_pump.h"
#include "ashe/message_loop/message_pump_dispatcher.h"
#include "ashe/win/message_window.h"

namespace ashe {
class ASHE_API MessagePumpForWin final : public MessagePump {
   public:
    MessagePumpForWin();
    ~MessagePumpForWin() final;

    // MessagePump methods:
    void run(Delegate* delegate) final;
    void quit() final;
    void scheduleWork() final;
    void scheduleDelayedWork(const TimePoint& delayed_work_time) final;

    void runWithDispatcher(Delegate* delegate, MessagePumpDispatcher* dispatcher);

   private:
    struct RunState {
        Delegate* delegate;
        MessagePumpDispatcher* dispatcher;

        // Used to flag that the current run() invocation should return ASAP.
        bool should_quit;

        // Used to count how many run() invocations are on the stack.
        int run_depth;
    };

    // State used with |work_state_| variable.
    enum WorkState {
        READY = 0,      // Ready to accept new work.
        HAVE_WORK = 1,  // New work has been signalled.
        WORKING = 2     // Handling the work.
    };

    void doRunLoop();
    bool onMessage(UINT message, WPARAM wparam, LPARAM lparam, LRESULT& result);
    void waitForWork();
    void handleWorkMessage();
    void handleTimerMessage();
    bool processNextWindowsMessage();
    bool processMessageHelper(const MSG& msg);
    bool processPumpReplacementMessage();
    int currentDelay() const;

    win::MessageWindow message_window_;

    // The time at which delayed work should run.
    TimePoint delayed_work_time_;

    // A value used to indicate if there is a kMsgDoWork message pending in the Windows Message
    // queue. There is at most one such message, and it can drive execution of tasks when a native
    // message pump is running.
    LONG work_state_ = READY;

    // State for the current invocation of Run.
    RunState* state_ = nullptr;

    ASHE_DISALLOW_COPY_AND_ASSIGN(MessagePumpForWin);
};
}  // namespace ashe
#endif  // ASHE_WIN

#endif  // ASHE_MESSAGE_LOOP_MESSAGE_PUMP_WIN_H_
