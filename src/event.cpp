#include "ashe/config.h"
#include "ashe/event.h"

#ifdef max
#undef max
#endif

namespace ashe {
Event::Event(bool isSet) noexcept :
    is_set_(isSet) {}

void Event::set() {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = true;
    setted_cond_var_.notify_all();
}

void Event::set(int64_t d) {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = true;
    data_ = d;
    setted_cond_var_.notify_all();
}

void Event::cancel() {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_cancelld_ = true;
    setted_cond_var_.notify_all();
}

void Event::unCancel() {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_cancelld_ = false;
    setted_cond_var_.notify_all();
}

void Event::unset() {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = false;
    setted_cond_var_.notify_all();
}

void Event::unset(int64_t d) {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = false;
    data_ = d;
    setted_cond_var_.notify_all();
}

void Event::reset() {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = false;
    is_cancelld_ = false;
    setted_cond_var_.notify_all();
}

void Event::reset(int64_t d) {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = false;
    is_cancelld_ = false;
    data_ = d;
    setted_cond_var_.notify_all();
}

bool Event::isSet() {
    std::unique_lock<std::mutex> ul(set_mutex_);
    return is_set_;
}

bool Event::isCancelled() {
    std::unique_lock<std::mutex> ul(set_mutex_);
    return is_cancelld_;
}

bool Event::wait(int64_t millseconds) {
    std::unique_lock<std::mutex> ul(set_mutex_);
    int64_t m = (millseconds >= 0 ? millseconds : std::chrono::duration_values<int64_t>::max());
    setted_cond_var_.wait_for(ul, std::chrono::milliseconds(m), [this] { return (is_set_ || is_cancelld_); });
    return is_set_;
}

bool Event::wait(int64_t& d, int64_t millseconds /*= -1*/) {
    std::unique_lock<std::mutex> ul(set_mutex_);
    int64_t m = (millseconds >= 0 ? millseconds : std::chrono::duration_values<int64_t>::max());
    setted_cond_var_.wait_for(ul, std::chrono::milliseconds(m), [this] { return (is_set_ || is_cancelld_); });
    d = data_;
    return is_set_;
}
}  // namespace ashe
