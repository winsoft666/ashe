#ifndef ASHE_THREAD_H_
#define ASHE_THREAD_H_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include "ashe/message_loop/message_loop.h"
#include <atomic>
#include <condition_variable>
#include <thread>

namespace ashe {
class ASHE_API Thread {
   public:
    Thread() = default;
    ~Thread();

    class Delegate {
       public:
        virtual ~Delegate() = default;

        // Called just prior to starting the message loop.
        virtual void onBeforeThreadRunning() {}

        virtual void onThreadRunning(MessageLoop* message_loop);

        // Called just after the message loop ends.
        virtual void onAfterThreadRunning() {}
    };

    // Starts the thread.
    void start(MessageLoop::Type messageLoopType, Delegate* delegate = nullptr);

    // Signals the thread to exit in the near future.
    void stopSoon();

    // Signals the thread to exit and returns once the thread has exited.
    // After this method returns, the Thread object is completely reset and may be used
    // as if it were newly constructed (i.e., Start may be called again).
    // Stop may be called multiple times and is simply ignored if the thread is already stopped.
    void stop();

    void join();

    inline bool isRunning() const { return running_; }
    inline MessageLoop* messageLoop() const { return messageLoop_; }
    inline std::shared_ptr<TaskRunner> taskRunner() const { return messageLoop_ ? messageLoop_->taskRunner() : nullptr; }

#if defined(ASHE_WIN)
    inline uint32_t threadId() const { return thread_id_; }

    enum class Priority {
        ABOVE_NORMAL = THREAD_PRIORITY_ABOVE_NORMAL,
        BELOW_NORMAL = THREAD_PRIORITY_BELOW_NORMAL,
        HIGHEST = THREAD_PRIORITY_HIGHEST,
        IDLE = THREAD_PRIORITY_IDLE,
        LOWEST = THREAD_PRIORITY_LOWEST,
        NORMAL = THREAD_PRIORITY_NORMAL
    };

    bool setPriority(Priority priority);

#endif  // defined(ASHE_WIN)

   private:
    void threadMain(MessageLoop::Type message_loop_type);

    Delegate* delegate_ = nullptr;

    enum class State {
        STARTING,
        STARTED,
        STOPPING,
        STOPPED
    };

    std::atomic<State> state_ = State::STOPPED;

    std::thread thread_;

#if defined(ASHE_WIN)
    uint32_t thread_id_ = 0;
#endif  // defined(ASHE_WIN)

    // True while inside of Run().
    bool running_ = false;
    std::mutex runningLock_;
    std::condition_variable runningEvent_;

    MessageLoop* messageLoop_ = nullptr;

    ASHE_DISALLOW_COPY(Thread);
};
}  // namespace ashe
#endif  // ASHE_THREAD_H_
