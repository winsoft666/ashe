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

#ifndef ASHE_MESSAGE_LOOP_MESSAGE_LOOP_TASK_RUNNER_H_
#define ASHE_MESSAGE_LOOP_MESSAGE_LOOP_TASK_RUNNER_H_
#pragma once

#include "ashe/config.h"
#include "ashe/compiler_specific.h"
#include "ashe/task_runner.h"
#include "ashe/message_loop/message_loop.h"
#include "ashe/message_loop/pending_task.h"
#if ASHE_CPP_STANDARD_VER >= 201703L
#include <shared_mutex>
#else
#include <mutex>
#endif
#include <thread>

namespace ashe {
class ASHE_API MessageLoopTaskRunner final : public TaskRunner {
   public:
    static std::shared_ptr<TaskRunner> current();

    // TaskRunner implementation.
    bool belongsToCurrentThread() const final;
    void postTask(Callback callback) final;
    void postDelayedTask(Callback callback, const Milliseconds& delay) final;
    void postNonNestableTask(Callback callback) final;
    void postNonNestableDelayedTask(Callback callback, const Milliseconds& delay) final;
    void postQuit() final;

   private:
    friend class MessageLoop;

    explicit MessageLoopTaskRunner(MessageLoop* loop);

    // Called directly by MessageLoop::~MessageLoop.
    void willDestroyCurrentMessageLoop();

    MessageLoop* loop_;
#if ASHE_CPP_STANDARD_VER >= 201703L
    mutable std::shared_mutex loopLock_;
#else
    mutable std::mutex loopLock_;
#endif
    std::thread::id threadId_;

    ASHE_DISALLOW_COPY_AND_ASSIGN(MessageLoopTaskRunner);
};

}  // namespace ashe

#endif  // ASHE_MESSAGE_LOOP_MESSAGE_LOOP_TASK_RUNNER_H_
