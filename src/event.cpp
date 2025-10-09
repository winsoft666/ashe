#include "ashe/config.h"
#include "ashe/event.h"

#ifdef max
#undef max
#endif

namespace ashe {
Event::Event(bool isSet) noexcept :
    is_set_(isSet) {}

void Event::set() {
    {
        std::unique_lock<std::mutex> ul(set_lock_);
        is_set_ = true;
    }
    setted_event_.notify_all();
}

void Event::set(int64_t d) {
    {
        std::unique_lock<std::mutex> ul(set_lock_);
        is_set_ = true;
        data_ = d;
    }
    setted_event_.notify_all();
}

void Event::unset() {
    {
        std::unique_lock<std::mutex> ul(set_lock_);
        is_set_ = false;
    }
    setted_event_.notify_all();
}

void Event::unset(int64_t d) {
    {
        std::unique_lock<std::mutex> ul(set_lock_);
        is_set_ = false;
        data_ = d;
    }
    setted_event_.notify_all();
}

void Event::reset() {
    {
        std::unique_lock<std::mutex> ul(set_lock_);
        is_set_ = false;
    }
    setted_event_.notify_all();
}

void Event::reset(int64_t d) {
    {
        std::unique_lock<std::mutex> ul(set_lock_);
        is_set_ = false;
        data_ = d;
    }
    setted_event_.notify_all();
}

bool Event::isSet() const {
    return is_set_;
}

bool Event::wait(int64_t millseconds) {
    std::unique_lock<std::mutex> ul(set_lock_);
    if (millseconds >= 0) {
        setted_event_.wait_for(ul, std::chrono::milliseconds(millseconds), [this] { return is_set_; });
    } else {
        setted_event_.wait(ul, [this] { return is_set_; });
    }
    return is_set_;
}

bool Event::wait(int64_t& d, int64_t millseconds /*= -1*/) {
    std::unique_lock<std::mutex> ul(set_lock_);
    if (millseconds >= 0) {
        setted_event_.wait_for(ul, std::chrono::milliseconds(millseconds), [this] { return is_set_; });
    } else {
        setted_event_.wait(ul, [this] { return is_set_; });
    }
    d = data_;
    return is_set_;
}
}  // namespace ashe
