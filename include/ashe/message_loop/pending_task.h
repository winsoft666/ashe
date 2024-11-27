#ifndef ASHE_MESSAGE_LOOP_PENDING_TASK_H_
#define ASHE_MESSAGE_LOOP_PENDING_TASK_H_
#pragma once

#include "ashe/config.h"
#include <chrono>
#include <functional>
#include <queue>

namespace ashe {
// Contains data about a pending task. Stored in TaskQueue and DelayedTaskQueue for use by classes
// that queue and execute tasks.
class ASHE_API PendingTask {
   public:
    using Callback = std::function<void()>;
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    PendingTask(Callback&& callback,
                TimePoint delayed_run_time,
                bool nestable,
                int sequence_num = 0);
    ~PendingTask() = default;

    // Used to support sorting.
    bool operator<(const PendingTask& other) const;

    // The task to run.
    Callback callback;

    // Secondary sort key for run time.
    int sequence_num;

    TimePoint delayed_run_time;

    // OK to dispatch from a nested loop.
    bool nestable;
};

// Wrapper around std::queue specialized for PendingTask which adds a Swap helper method.
class ASHE_API TaskQueue : public std::queue<PendingTask> {
   public:
    void Swap(TaskQueue* queue) {
        c.swap(queue->c);  // Calls std::deque::swap.
    }
};

// PendingTasks are sorted by their |delayed_run_time| property.
using DelayedTaskQueue = std::priority_queue<PendingTask>;

}  // namespace ashe
#endif  // !ASHE_MESSAGE_LOOP_PENDING_TASK_H_
