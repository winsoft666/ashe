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

#ifndef ASHE_MESSAGE_LOOP_TASK_RUNNNER_H_
#define ASHE_MESSAGE_LOOP_TASK_RUNNNER_H_
#pragma once

#include "ashe/config.h"
#include <chrono>
#include <functional>
#include <memory>

namespace ashe {
class ASHE_API TaskRunner : public std::enable_shared_from_this<TaskRunner> {
   public:
    virtual ~TaskRunner() = default;

    using Callback = std::function<void()>;
    using Milliseconds = std::chrono::milliseconds;

    virtual bool belongsToCurrentThread() const = 0;
    virtual void postTask(Callback task) = 0;
    virtual void postDelayedTask(Callback callback, const Milliseconds& delay) = 0;
    virtual void postNonNestableTask(Callback callback) = 0;
    virtual void postNonNestableDelayedTask(Callback callback, const Milliseconds& delay) = 0;
    virtual void postQuit() = 0;

    template <class T>
    static void doDelete(const void* object) {
        delete static_cast<const T*>(object);
    }

    template <class T>
    void deleteSoon(const T* object) {
        deleteSoonInternal(&TaskRunner::doDelete<T>, object);
    }

    template <class T>
    void deleteSoon(std::unique_ptr<T> object) {
        deleteSoon(object.release());
    }

   private:
    void deleteSoonInternal(void (*deleter)(const void*), const void* object);
};
}  // namespace ashe

#endif  // !ASHE_MESSAGE_LOOP_TASK_RUNNNER_H_
