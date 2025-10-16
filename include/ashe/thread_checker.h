/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2020~2024 winsoft666 <winsoft666@outlook.com>.
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef ASHE_THREAD_CHECKER_H_
#define ASHE_THREAD_CHECKER_H_
#pragma once

#include "ashe/config.h"
#include "ashe/macros.h"
#include <thread>
#include <mutex>
#include "ashe/logging.h"

namespace ashe {
// 检查函数是否运行在合适的线程
// 
// 推荐使用 THREAD_CHECKER、DCHECK_CALLED_ON_VALID_THREAD、DETACH_FROM_THREAD 宏，仅在Debug环境下检查
// 
// 使用方法如下:
//   class MyClass
//   {
//   public:
//       MyClass() {
//       }
//
//       ~MyClass() {
//           // 检查析构函数是否与构造函数在同一个线程中被调用
//           DCHECK_CALLED_ON_VALID_THREAD(my_thread_checker_);
//       }
//
//       void MyMethod() {
//           // 检查MyMethod函数是否与MyClass构造函数在同一个线程中被调用
//           DCHECK_CALLED_ON_VALID_THREAD(my_thread_checker_);
//       }
//
//    private:
//       THREAD_CHECKER(my_thread_checker_);
//   }
//
class ASHE_API ThreadChecker {
   public:
    ThreadChecker();

    bool calledOnValidThread() const;
    void detachFromThread();

   private:
    void ensureAssigned();

    std::thread::id thread_id_;
    mutable std::mutex thread_id_lock_;

    ASHE_DISALLOW_COPY_AND_ASSIGN(ThreadChecker);
};
}  // namespace ashe

#ifndef NDEBUG
#define THREAD_CHECKER(name) ::ashe::ThreadChecker name
#define DCHECK_CALLED_ON_VALID_THREAD(name) CHECK((name).calledOnValidThread()) << "this call is running on invalid thread"
#define DETACH_FROM_THREAD(name) (name).detachFromThread()
#else  // NDEBUG
#define THREAD_CHECKER(name)
#define DCHECK_CALLED_ON_VALID_THREAD(name)
#define DETACH_FROM_THREAD(name)
#endif  // NDEBUG

#endif  // !ASHE_THREAD_CHECKER_H_
