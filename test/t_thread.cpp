#include "catch.hpp"
#include <thread>
#include "ashe/thread.h"

using namespace ashe;

TEST_CASE("Thread-1", "") {
    class DelegateA : public Thread::Delegate {
       public:
        DelegateA(Thread* t) {
            t_ = t;
        }

        virtual void onBeforeThreadRunning() {
            std::thread t = std::thread([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                t_->taskRunner()->postQuit();
            });
            t.detach();
        }

        virtual void onAfterThreadRunning() {}

        Thread* t_ = nullptr;
    };

    Thread t1;
    t1.start(MessageLoop::Type::DEFAULT, new DelegateA(&t1));
    t1.join();

    REQUIRE(true);
}

TEST_CASE("Thread-2", "") {
    class DelegateA : public Thread::Delegate {
       public:
        DelegateA(Thread* t) {
            t_ = t;
        }

        virtual void onBeforeThreadRunning() {
            std::thread t = std::thread([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                t_->stopSoon();
            });
            t.detach();
        }

        virtual void onAfterThreadRunning() {}

        Thread* t_ = nullptr;
    };

    Thread t1;
    t1.start(MessageLoop::Type::DEFAULT, new DelegateA(&t1));
    t1.join();

    REQUIRE(true);
}

TEST_CASE("Thread-3", "") {
    Thread t1;
    t1.start(MessageLoop::Type::DEFAULT);

    Thread t2;
    t2.start(MessageLoop::Type::DEFAULT);
}
