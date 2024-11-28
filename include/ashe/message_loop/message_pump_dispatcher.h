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

#ifndef ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DISPATCHER_H_
#define ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DISPATCHER_H_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"

#if defined(ASHE_WIN)
#include <Windows.h>
#elif defined(ASHE_LINUX)
typedef union _XEvent XEvent;
#elif defined(ASHE_MACOS)
#if defined(__OBJC__)
@class NSEvent;
#else   // __OBJC__
class NSEvent;
#endif  // __OBJC__
#endif  // OS_*

namespace ashe {
#if defined(ASHE_WIN)
using NativeEvent = MSG;
#elif defined(ASHE_LINUX)
using NativeEvent = XEvent*;
#elif defined(ASHE_MACOS)
using NativeEvent = NSEvent*;
#else
#error Not implemented
#endif

// Dispatcher is used during a nested invocation of Run to dispatch events when
// |RunLoop(dispatcher).run()| is used.  If |RunLoop().run()| is invoked,
// MessageLoop does not dispatch events (or invoke TranslateMessage), rather
// every message is passed to Dispatcher's Dispatch method for dispatch. It is
// up to the Dispatcher whether or not to dispatch the event.
//
// The nested loop is exited by either posting a quit, or returning false from Dispatch.
class ASHE_API MessagePumpDispatcher {
   public:
    virtual ~MessagePumpDispatcher() = default;

    // Dispatches the event. If true is returned processing continues as
    // normal. If false is returned, the nested loop exits immediately.
    virtual bool dispatch(const NativeEvent& event) = 0;
};

}  // namespace ashe

#endif  // ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DISPATCHER_H_
