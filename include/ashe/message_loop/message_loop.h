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

#ifndef ASHE_MESSAGE_LOOP_MESSAGE_LOOP_H_
#define ASHE_MESSAGE_LOOP_MESSAGE_LOOP_H_

#include "ashe/arch.h"
#include "ashe/config.h"
#include "ashe/macros.h"
#include "ashe/message_loop/task_runner.h"
#include "ashe/message_loop/message_pump.h"
#include "ashe/message_loop/message_pump_dispatcher.h"
#include "ashe/message_loop/pending_task.h"
#include <memory>
#include <mutex>

namespace ashe {

class MessageLoopTaskRunner;
class MessagePumpForAsio;
class MessagePumpForWin;
class Thread;

class ASHE_API MessageLoop final : public MessagePump::Delegate {
   public:
    enum class Type {
        DEFAULT,
        //ASIO,
#if defined(ASHE_WIN)
        WIN
#endif  // defined(ASHE_WIN)
    };

    using Dispatcher = MessagePumpDispatcher;

    explicit MessageLoop(Type type = Type::DEFAULT);
    virtual ~MessageLoop() final;

    Type type() const { return type_; }

    void run(Dispatcher* dispatcher = nullptr);

    static MessageLoop* current();

#if defined(ASHE_WIN)
    MessagePumpForWin* pumpWin() const;
#endif  // defined(ASHE_WIN)

    //MessagePumpForAsio* pumpAsio() const;

    std::shared_ptr<TaskRunner> taskRunner() const;

   protected:
    friend class MessageLoopTaskRunner;
    friend class Thread;

    using Clock = MessagePump::Clock;
    using TimePoint = MessagePump::TimePoint;
    using Milliseconds = MessagePump::Milliseconds;

    void postTask(PendingTask::Callback callback);
    void postDelayedTask(PendingTask::Callback callback, const Milliseconds& delay);
    void postNonNestableTask(PendingTask::Callback callback);
    void postNonNestableDelayedTask(PendingTask::Callback callback, const Milliseconds& delay);

    PendingTask::Callback quitClosure();

    // Runs the specified PendingTask.
    void runTask(const PendingTask& pending_task);

    // Calls RunTask or queues the pending_task on the deferred task list if it cannot be run right
    // now. Returns true if the task was run.
    bool deferOrRunPendingTask(const PendingTask& pending_task);

    // Adds the pending task to delayed_work_queue_.
    void addToDelayedWorkQueue(PendingTask* pending_task);

    // Adds the pending task to our incoming_queue_.
    //
    // Caller retains ownership of |pending_task|, but this function will reset the value of
    // pending_task->task. This is needed to ensure that the posting call stack does not retain
    // pending_task->task beyond this function call.
    void addToIncomingQueue(PendingTask::Callback&& callback, const Milliseconds& delay, bool nestable);

    // Load tasks from the incoming_queue_ into work_queue_ if the latter is empty. The former
    // requires a lock to access, while the latter is directly accessible on this thread.
    void reloadWorkQueue();

    bool deletePendingTasks();

    // Calculates the time at which a PendingTask should run.
    static TimePoint calculateDelayedRuntime(const Milliseconds& delay);

    // MessagePump::Delegate methods:
    bool doWork() final;
    bool doDelayedWork(TimePoint* nextDelayedWorkTime) final;
    bool doIdleWork() final;

    const Type type_;

    // A recent snapshot of Clock::now(), used to check delayed_work_queue_.
    TimePoint recent_time_;

    // Contains delayed tasks, sorted by their 'delayed_run_time' property.
    DelayedTaskQueue delayedWorkQueue_;

    // A list of tasks that need to be processed by this instance.  Note that this queue is only
    // accessed (push/pop) by our current thread.
    TaskQueue work_queue_;

    // A queue of non-nestable tasks that we had to defer because when it came time to execute them
    // we were in a nested message loop. They will execute once we're out of nested message loops.
    TaskQueue deferredNonNestableWorkQueue_;

    // A recursion block that prevents accidentally running additional tasks when insider a
    // (accidentally induced?) nested message pump.
    bool nestableTasksAllowed_ = true;

    std::shared_ptr<MessagePump> pump_;

    TaskQueue incomingQueue_;
    std::mutex incomingQueueLock_;

    // The next sequence number to use for delayed tasks.
    int next_sequence_num_ = 0;

    std::shared_ptr<MessageLoopTaskRunner> proxy_;

   private:
    void quit();

    ASHE_DISALLOW_COPY(MessageLoop);
};
}  // namespace ashe
#endif  // ASHE_MESSAGE_LOOP_MESSAGE_LOOP_H_
