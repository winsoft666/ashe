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

#ifndef ASHE_AT_EXIT_H_
#define ASHE_AT_EXIT_H_
#pragma once
#include "ashe/config.h"
#include "ashe/macros.h"
#include <stack>
#include <mutex>
#include <functional>

namespace ashe {
// 该类提供了一种类似于 C 运行时库 atexit() 的功能，不同之处在于我们能够控制回调函数的执行时机。
// 该类通常与ashe::SingletonClass结合使用。
// 
// 使用方法很简单，在 main() 或 WinMain() 函数的早期，在栈上创建一个 AtExitManager 对象：
// int main(...) {
//    ashe::AtExitManager exit_manager;
//
// }
// 
// 当 exit_manager 对象超出作用域时，所有注册的回调函数将被调用。
// 
// 注意：
// 1. 一个程序只创建一个AtExitManager实例。
// 2. 回调函数的执行顺序与注册顺序相反（后注册的先执行）。
// 3. 如果需要，可以在调用 ProcessCallbacksNow() 后继续注册新的回调函数。
// 4. 回调函数应该尽量简单，避免执行耗时操作，以免阻塞程序退出。
// 5. 如果在回调函数中抛出异常，程序将调用 std::terminate() 并终止运行。
// 6. 该类是线程安全的，可以在多个线程中注册回调函数。
// 7. 如果需要在程序退出前立即执行所有注册的回调函数，可以调用静态方法 ProcessCallbacksNow()。
// 8. 该类在析构函数中执行回调函数，因此确保 AtExitManager 对象的生命周期覆盖所有需要注册回调函数的代码。
//
class ASHE_API AtExitManager {
   public:
    typedef std::function<void(void)> AtExitCallbackType;

    AtExitManager();

    ~AtExitManager();

    // 线程安全
    // 注册回调函数，以便在退出时（析构或手动调用ProcessCallbacksNow）调用该函数。
    static void RegisterCallback(AtExitCallbackType func);

    // 线程安全
    // 手动立即执行所有注册的回调函数（后注册的先执行），可以在该函数执行完成后，再次调用RegisterCallback注册新的回调函数。
    static void ProcessCallbacksNow();

   private:
    std::mutex m_;
    std::stack<AtExitCallbackType> stack_;

    ASHE_DISALLOW_COPY_AND_MOVE(AtExitManager);
};

}  // namespace ashe

#endif  // ASHE_AT_EXIT_H_
