#ifndef ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DISPATCHER_H_
#define ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DISPATCHER_H_
#pragma once

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
// The nested loop is exited by either posting a quit, or returning false
// from Dispatch.
class MessagePumpDispatcher {
   public:
    virtual ~MessagePumpDispatcher() = default;

    // Dispatches the event. If true is returned processing continues as
    // normal. If false is returned, the nested loop exits immediately.
    virtual bool dispatch(const NativeEvent& event) = 0;
};

}  // namespace ashe

#endif  // ASHE_MESSAGE_LOOP_MESSAGE_PUMP_DISPATCHER_H_
