#include "ashe/thread_checker.h"

namespace ashe {

ThreadChecker::ThreadChecker() {
    ensureAssigned();
}

bool ThreadChecker::calledOnValidThread() const {
    std::lock_guard<std::mutex> lock(thread_id_lock_);
    return thread_id_ == std::this_thread::get_id();
}

void ThreadChecker::detachFromThread() {
    std::lock_guard<std::mutex> lock(thread_id_lock_);
    thread_id_ = std::thread::id();
}

void ThreadChecker::ensureAssigned() {
    std::lock_guard<std::mutex> lock(thread_id_lock_);
    thread_id_ = std::this_thread::get_id();
}

}  // namespace ashe
