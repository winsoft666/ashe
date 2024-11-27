#ifndef ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DEFAULT_H_
#define ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DEFAULT_H_
#pragma once

#include "ashe/macros.h"
#include <condition_variable>
#include <mutex>
#include "ashe/message_loop/message_pump.h"

namespace ashe {

class MessagePumpDefault final : public MessagePump {
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
    bool keep_running_ = true;

    // Used to sleep until there is more work to do.
    std::condition_variable event_;

    bool have_work_ = false;
    std::mutex have_work_lock_;

    // The time at which we should call doDelayedWork.
    TimePoint delayed_work_time_;

    ASHE_DISALLOW_COPY(MessagePumpDefault);
};

}  // namespace ashe

#endif  // ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DEFAULT_H_
