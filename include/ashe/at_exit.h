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

// This class provides a facility similar to the CRT atexit(), except that
// we control when the callbacks are executed. Under Windows for a DLL they
// happen at a really bad time and under the loader lock. This facility is
// mostly used by base::Singleton.
//
// The usage is simple. 
// Early in the main() or WinMain() scope create an
// AtExitManager object on the stack:
// int main(...) {
//    ashe::AtExitManager exit_manager;
//
// }
// 
// When the exit_manager object goes out of scope, all the registered callbacks will be called.

class ASHE_API AtExitManager {
   public:
    typedef std::function<void(void)> AtExitCallbackType;

    AtExitManager();

    // The dtor calls all the registered callbacks.
    // Do not try to register more callbacks after this point.
    ~AtExitManager();

    // Registers the specified function to be called at exit.
    static void RegisterCallback(AtExitCallbackType func);

    // Calls the functions registered with RegisterCallback in LIFO order.
    // It is possible to register new callbacks after calling this function.
    static void ProcessCallbacksNow();

   private:
    std::mutex m_;
    std::stack<AtExitCallbackType> stack_;

    ASHE_DISALLOW_COPY_MOVE(AtExitManager);
};

}  // namespace ashe

#endif  // ASHE_AT_EXIT_H_
