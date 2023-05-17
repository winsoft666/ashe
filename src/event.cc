#include "ashe/config.hpp"
#include "ashe/event.hpp"

#ifdef max
#undef max
#endif

ashe::Event::Event(bool isSet) :
    is_set_(isSet) {}

void ashe::Event::set() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = true;
    setted_cond_var_.notify_all();
}

void ashe::Event::set(const DataType& d) noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = true;
    data_ = d;
    setted_cond_var_.notify_all();
}

void ashe::Event::cancel() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_cancelld_ = true;
    setted_cond_var_.notify_all();
}

void ashe::Event::unCancel() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_cancelld_ = false;
    setted_cond_var_.notify_all();
}

void ashe::Event::unset() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = false;
    setted_cond_var_.notify_all();
}

void ashe::Event::unset(const DataType& d) noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = false;
    data_ = d;
    setted_cond_var_.notify_all();
}

void ashe::Event::reset() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = false;
    is_cancelld_ = false;
    setted_cond_var_.notify_all();
}

void ashe::Event::reset(const DataType& d) noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    is_set_ = false;
    is_cancelld_ = false;
    data_ = d;
    setted_cond_var_.notify_all();
}

bool ashe::Event::isSet() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    return is_set_;
}

bool ashe::Event::isCancelled() noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    return is_cancelld_;
}

bool ashe::Event::wait(int64_t millseconds) noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    int64_t m = (millseconds >= 0 ? millseconds : std::chrono::duration_values<int64_t>::max());
    setted_cond_var_.wait_for(ul, std::chrono::milliseconds(m), [this] { return (is_set_ || is_cancelld_); });
    return is_set_;
}

bool ashe::Event::wait(DataType& d, int64_t millseconds /*= -1*/) noexcept {
    std::unique_lock<std::mutex> ul(set_mutex_);
    int64_t m = (millseconds >= 0 ? millseconds : std::chrono::duration_values<int64_t>::max());
    setted_cond_var_.wait_for(ul, std::chrono::milliseconds(m), [this] { return (is_set_ || is_cancelld_); });
    d = data_;
    return is_set_;
}
