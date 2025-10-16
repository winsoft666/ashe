#include "ashe/config.h"
#include "ashe/timer_manager.h"
#include "ashe/macros.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <vector>
#include <stack>
#include <set>
#include "ashe/logging.h"

namespace ashe {
namespace timer_detail {
struct TimerData {
    std::size_t id;
    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::microseconds period;
    TimerManager::TimerHandler handler;
    bool valid;
    TimerData() noexcept :
        id(0),
        start(std::chrono::microseconds::zero()),
        period(std::chrono::microseconds::zero()),
        handler(nullptr),
        valid(false) {
    }

    template <typename Func>
    TimerData(std::size_t id, std::chrono::time_point<std::chrono::steady_clock> start, std::chrono::microseconds period, Func&& handler) :
        id(id), start(start), period(period), handler(std::forward<Func>(handler)), valid(true) {
    }

    TimerData(TimerData&& r) = default;
    TimerData& operator=(TimerData&& ev) = default;
    TimerData(const TimerData& r) = delete;
    TimerData& operator=(const TimerData& r) = delete;
};

struct TimeEvent {
    std::chrono::time_point<std::chrono::steady_clock> next;
    std::size_t ref;
};

inline bool operator<(const TimeEvent& l, const TimeEvent& r) {
    return l.next < r.next;
}
}  // namespace timer_detail

class TimerManager::Private {
   public:
    std::mutex m_;
    std::condition_variable cond_;
    std::thread worker_;
    bool done_ = false;
    std::vector<timer_detail::TimerData> events_;

    // Sorted queue that has the next timeout at its top.
    std::multiset<timer_detail::TimeEvent> time_events_;

    // A list of ids to be re-used. If possible, ids are used from this pool.
    std::stack<std::size_t> free_ids_;
};

TimerManager::TimerManager() noexcept :
    p_(std::make_unique<TimerManager::Private>()) {
}

TimerManager::~TimerManager() {
    destory();
}

void TimerManager::init() {
    std::unique_lock<std::mutex> lock(p_->m_);
    p_->worker_ = std::thread([this] {
        try {
            run();
        } catch (std::exception& e) {
            DLOG(LS_FATAL) << "exception occurred: " << e.what();
        }
    });
}

void TimerManager::destory() {
    if (p_) {
        std::unique_lock<std::mutex> lock(p_->m_);
        p_->done_ = true;
        lock.unlock();
        p_->cond_.notify_all();
        try {
            if (p_->worker_.joinable())
                p_->worker_.join();
        } catch (std::exception& e) {
            DLOG(LS_FATAL) << "exception occurred: " << e.what();
        }
        p_->events_.clear();
        p_->time_events_.clear();
        while (!p_->free_ids_.empty()) {
            p_->free_ids_.pop();
        }
        p_.reset();
    }
}

TimerManager::TimerId TimerManager::add(const SteadyClock& when, const TimerHandler& handler, const std::chrono::microseconds& period) {
    std::unique_lock<std::mutex> lock(p_->m_);
    std::size_t id = 0;
    // Add a new event.
    // Prefer an existing and free id. If none is available, add a new one.
    if (p_->free_ids_.empty()) {
        id = p_->events_.size();
        timer_detail::TimerData e(id, when, period, handler);
        p_->events_.push_back(std::move(e));
    }
    else {
        id = p_->free_ids_.top();
        p_->free_ids_.pop();
        timer_detail::TimerData e(id, when, period, handler);
        p_->events_[id] = std::move(e);
    }
    p_->time_events_.insert(timer_detail::TimeEvent{when, id});
    lock.unlock();
    p_->cond_.notify_all();
    return id;
}

TimerManager::TimerId TimerManager::add(const uint64_t afterMicroseconds, const TimerHandler& handler, const uint64_t periodMicroseconds) {
    return add(std::chrono::microseconds(afterMicroseconds), handler, std::chrono::microseconds(periodMicroseconds));
}

bool TimerManager::remove(TimerManager::TimerId id) {
    std::unique_lock<std::mutex> lock(p_->m_);
    if (p_->events_.empty() || p_->events_.size() <= id) {
        return false;
    }
    p_->events_[id].valid = false;
    auto it = std::find_if(p_->time_events_.begin(), p_->time_events_.end(), [&](const timer_detail::TimeEvent& te) { return te.ref == id; });
    if (it != p_->time_events_.end()) {
        p_->free_ids_.push(it->ref);
        p_->time_events_.erase(it);
    }
    lock.unlock();
    p_->cond_.notify_all();
    return true;
}

void TimerManager::run() {
    std::unique_lock<std::mutex> lock(p_->m_);

    while (!p_->done_) {
        if (p_->time_events_.empty()) {
            // Wait for work
            p_->cond_.wait(lock);
        }
        else {
            timer_detail::TimeEvent te = *(p_->time_events_.begin());
            if (std::chrono::steady_clock::now() >= te.next) {
                // Remove time event
                p_->time_events_.erase(p_->time_events_.begin());

                // Invoke the handler
                lock.unlock();
                try {
                    p_->events_[te.ref].handler(te.ref);
                } catch (std::exception& e) {
                    ASHE_UNUSED(e);
                }
                lock.lock();

                if (p_->events_[te.ref].valid && p_->events_[te.ref].period.count() > 0) {
                    // The event is valid and a periodic timer.
                    te.next += p_->events_[te.ref].period;
                    p_->time_events_.insert(te);
                }
                else {
                    // The event is either no longer valid because it was removed in the
                    // callback, or it is a one-shot timer.
                    p_->events_[te.ref].valid = false;
                    p_->free_ids_.push(te.ref);
                }
            }
            else {
                p_->cond_.wait_until(lock, te.next);
            }
        }
    }
}
}  // namespace ashe