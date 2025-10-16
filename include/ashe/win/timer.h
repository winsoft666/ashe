/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022~2023 winsoft666 <winsoft666@outlook.com>.
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

#ifndef ASHE_WIN_TIMER_HPP_
#define ASHE_WIN_TIMER_HPP_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#include <functional>

namespace ashe {
namespace win {
// 基于Windows Timer Queue 实现的毫秒级定时器
// 通常不直接使用该类，而是使用TTimer或Timer
//
class ASHE_API WinTimerBase {
   public:
    WinTimerBase();

    virtual ~WinTimerBase();

    static void CALLBACK TimerProc(void* param, BOOLEAN timerCalled);

    // About dwFlags, see:
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms682485(v=vs.85).aspx
    //
    bool start(unsigned int ulInterval,  // ulInterval in ms
               bool bImmediately,
               bool bOnce,
               unsigned long dwFlags = WT_EXECUTELONGFUNCTION);

    void stop(bool bWait);

    virtual void onTimedEvent() {}

   private:
    class Private;
    Private* p_ = nullptr;
};

// 模板方式
//
template <class T>
class TTimer : public WinTimerBase {
   public:
    typedef void (T::*POnTimer)(void);

    TTimer() {
        m_pClass = NULL;
        m_pfnOnTimer = NULL;
    }

    void setTimedEvent(T* pClass, POnTimer pFunc) {
        m_pClass = pClass;
        m_pfnOnTimer = pFunc;
    }

   protected:
    void onTimedEvent() override {
        if (m_pfnOnTimer && m_pClass) {
            (m_pClass->*m_pfnOnTimer)();
        }
    }

   private:
    T* m_pClass;
    POnTimer m_pfnOnTimer;
};

// 使用std::function方式，支持Lambda表达式
//
class ASHE_API Timer : public WinTimerBase {
   public:
    typedef std::function<void()> FN_CB;
    Timer() {}

    Timer(FN_CB cb) { setTimedEvent(cb); }

    void setTimedEvent(FN_CB cb) { m_cb = cb; }

   protected:
    void onTimedEvent() override {
        if (m_cb) {
            m_cb();
        }
    }

   private:
    FN_CB m_cb;
};
}  // namespace win
}  // namespace ashe
#endif  // !ASHE_WIN_TIMER_HPP_
