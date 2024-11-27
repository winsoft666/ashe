#include "ashe/thread.h"
#include "ashe/scoped_object.h"
#include "ashe/check_failure.h"

namespace ashe {
Thread::~Thread() {
    stop();
}

void Thread::start(MessageLoop::Type message_loop_type, Delegate* delegate) {
    ASHE_CHECK_FAILURE(!message_loop_, nullptr);

    delegate_ = delegate;
    state_ = State::STARTING;

    thread_ = std::thread(&Thread::threadMain, this, message_loop_type);

    std::unique_lock<std::mutex> lock(running_lock_);
    while (!running_)
        running_event_.wait(lock);

    state_ = State::STARTED;

    ASHE_CHECK_FAILURE(message_loop_, nullptr);
}

void Thread::stopSoon() {
    if (state_ == State::STOPPING || !message_loop_)
        return;

    state_ = State::STOPPING;

    message_loop_->postTask(message_loop_->quitClosure());
}

void Thread::stop() {
    if (state_ == State::STOPPED)
        return;

    stopSoon();

#if defined(ASHE_WIN)
    if (message_loop_ && message_loop_->type() == MessageLoop::Type::WIN)
        PostThreadMessageW(thread_id_, WM_QUIT, 0, 0);
#endif  // defined(ASHE_WIN)

    join();

    // The thread should NULL message_loop_ on exit.
    ASHE_CHECK_FAILURE(!message_loop_, nullptr);

    delegate_ = nullptr;
}

void Thread::join() {
    if (state_ == State::STOPPED)
        return;

    // Wait for the thread to exit.
    if (thread_.joinable())
        thread_.join();

    // The thread no longer needs to be joined.
    state_ = State::STOPPED;
}

void Thread::Delegate::onThreadRunning(MessageLoop* message_loop) {
    message_loop->run();
}

void Thread::threadMain(MessageLoop::Type message_loop_type) {
    // The message loop for this thread.
    MessageLoop message_loop(message_loop_type);

    // Complete the initialization of our Thread object.
    message_loop_ = &message_loop;

#if defined(ASHE_WIN)
    ScopedComInitialize com_initializer;
    thread_id_ = GetCurrentThreadId();
#endif  // defined(ASHE_WIN)

    // Let the thread do extra initialization.
    // Let's do this before signaling we are started.
    if (delegate_)
        delegate_->onBeforeThreadRunning();

    {
        std::unique_lock<std::mutex> lock(running_lock_);
        running_ = true;
    }

    running_event_.notify_one();

    if (delegate_) {
        delegate_->onThreadRunning(message_loop_);
    }
    else {
        message_loop_->run();
    }

    {
        std::unique_lock<std::mutex> lock(running_lock_);
        running_ = false;
    }

    // Let the thread do extra cleanup.
    if (delegate_)
        delegate_->onAfterThreadRunning();

    // We can't receive messages anymore.
    message_loop_ = nullptr;
}

#if defined(ASHE_WIN)
bool Thread::setPriority(Priority priority) {
    if (!SetThreadPriority(thread_.native_handle(), static_cast<int>(priority))) {
        ASHE_CHECK_FAILURE(false, "Unable to set thread priority");
        return false;
    }

    return true;
}
#endif  // defined(ASHE_WIN)

}  // namespace ashe
