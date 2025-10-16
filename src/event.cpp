#include "ashe/config.h"
#include "ashe/event.h"

#ifdef max
#undef max
#endif

namespace ashe {
Event::Event(bool isSet) noexcept :
    is_set_(isSet) {
}

void Event::set() {
    {
        std::unique_lock<std::mutex> ul(set_lock_);
        is_set_ = true;
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

bool Event::isSet() const {
    std::unique_lock<std::mutex> ul(set_lock_);
    return is_set_;
}

bool Event::wait(int64_t millseconds) {
    std::unique_lock<std::mutex> ul(set_lock_);
    if (millseconds >= 0) {
        setted_event_.wait_for(ul, std::chrono::milliseconds(millseconds), [this] { return is_set_; });
    }
    else {
        setted_event_.wait(ul, [this] { return is_set_; });
    }
    return is_set_;
}
}  // namespace ashe
