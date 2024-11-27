#ifndef ASHE_THREAD_CHECKER_H_
#define ASHE_THREAD_CHECKER_H_
#pragma once

#include "ashe/macros.h"
#include <thread>
#include <mutex>
#include "ashe/check_failure.h"

// ThreadChecker is a helper class used to help verify that some methods of a
// class are called from the same thread (for thread-affinity).
//
// Use the macros below instead of the ThreadChecker directly so that the unused
// member doesn't result in an extra byte (four when padded) per instance in
// production.
//
// Usage:
//   class MyClass
//   {
//   public:
//       MyClass()
//       {
//           // It's sometimes useful to detach on construction for objects that are
//           // constructed in one place and forever after used from another
//           // thread.
//           DETACH_FROM_THREAD(my_thread_checker_);
//       }
//
//       ~MyClass()
//       {
//           // ThreadChecker doesn't automatically check it's destroyed on origin
//           // thread for the same reason it's sometimes detached in the
//           // constructor. It's okay to destroy off thread if the owner otherwise
//           // knows usage on the associated thread is done. If you're not
//           // detaching in the constructor, you probably want to explicitly check
//           // in the destructor.
//           DCHECK_CALLED_ON_VALID_THREAD(my_thread_checker_);
//       }
//
//       void MyMethod()
//       {
//           DCHECK_CALLED_ON_VALID_THREAD(my_thread_checker_);
//           ... (do stuff) ...
//       }
//
//    private:
//       THREAD_CHECKER(my_thread_checker_);
//   }

namespace ashe {
class ThreadChecker {
   public:
    ThreadChecker();
    ~ThreadChecker() = default;

    bool calledOnValidThread() const;
    void detachFromThread();

   private:
    void ensureAssigned();

    std::thread::id thread_id_;
    mutable std::mutex thread_id_lock_;

    ASHE_DISALLOW_COPY(ThreadChecker);
};
}  // namespace ashe

#ifndef NDEBUG
#define THREAD_CHECKER(name) ::ashe::ThreadChecker name
#define DCHECK_CALLED_ON_VALID_THREAD(name) ASHE_CHECK_FAILURE((name).calledOnValidThread(), "this call is running on invalid thread")
#define DETACH_FROM_THREAD(name) (name).detachFromThread()
#else  // NDEBUG
#define THREAD_CHECKER(name)
#define DCHECK_CALLED_ON_VALID_THREAD(name)
#define DETACH_FROM_THREAD(name)
#endif  // NDEBUG

#endif  // !ASHE_THREAD_CHECKER_H_
