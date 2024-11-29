#include "ashe/message_loop/message_pump_default.h"
#include "ashe/logging.h"

namespace ashe {
void MessagePumpDefault::run(Delegate* delegate) {
    DCHECK(keepRunning_) << "Quit must have been called outside of Run!";

    for (;;) {
        bool did_work = delegate->doWork();
        if (!keepRunning_)
            break;

        did_work |= delegate->doDelayedWork(&delayedWorkTime_);
        if (!keepRunning_)
            break;

        if (did_work)
            continue;

        did_work = delegate->doIdleWork();
        if (!keepRunning_)
            break;

        if (did_work)
            continue;

        if (delayedWorkTime_ == TimePoint()) {
            std::unique_lock<std::mutex> lock(haveWorkLock_);

            while (!have_work_)
                event_.wait(lock);

            have_work_ = false;
        }
        else {
            Milliseconds delay = std::chrono::duration_cast<Milliseconds>(
                delayedWorkTime_ - Clock::now());

            if (delay > Milliseconds::zero()) {
                std::unique_lock<std::mutex> lock(haveWorkLock_);

                do {
                    if (event_.wait_for(lock, delay) == std::cv_status::timeout)
                        break;

                    if (have_work_)
                        break;

                    // Recalculate the waiting interval.
                    delay = std::chrono::duration_cast<Milliseconds>(
                        delayedWorkTime_ - Clock::now());
                } while (delay > Milliseconds::zero());

                have_work_ = false;
            }
            else {
                // It looks like delayed_work_time_ indicates a time in the past, so we need to
                // call doDelayedWork now.
                delayedWorkTime_ = TimePoint();
            }
        }
    }

    keepRunning_ = true;
}

void MessagePumpDefault::quit() {
    keepRunning_ = false;
}

void MessagePumpDefault::scheduleWork() {
    {
        std::lock_guard<std::mutex> lock(haveWorkLock_);
        have_work_ = true;
    }

    // Since this can be called on any thread, we need to ensure that our Run loop wakes up.
    event_.notify_one();
}

void MessagePumpDefault::scheduleDelayedWork(const TimePoint& delayed_work_time) {
    // We know that we can't be blocked on Wait right now since this method can
    // only be called on the same thread as Run, so we only need to update our
    // record of how long to sleep when we do sleep.
    delayedWorkTime_ = delayed_work_time;
}
}  // namespace ashe
