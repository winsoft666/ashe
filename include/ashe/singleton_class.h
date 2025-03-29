/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2020~2025 winsoft666 <winsoft666@outlook.com>.
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

#ifndef ASHE_SINGLETON_CLASS_H_
#define ASHE_SINGLETON_CLASS_H_
#pragma once

#include "ashe/at_exit.h"

// Singleton class implement based on the Lazy Loading Mode.
//
namespace ashe {
template <class T, bool releaseAtExitManager = false>
class SingletonClass {
   public:
    static T* Instance() {
        static T* instance = nullptr;
        static std::once_flag oc;  // C++ 11
        std::call_once(oc, [&] {
            instance = new T();
            if (releaseAtExitManager) {
                AtExitManager::RegisterCallback([&]() {
                    delete instance;
                    instance = nullptr;
                });
            }
        });
        return instance;
    }

   protected:
    SingletonClass() = default;
    virtual ~SingletonClass() = default;
    SingletonClass(const SingletonClass&) = delete;
    SingletonClass& operator=(const SingletonClass&) = delete;
    SingletonClass(SingletonClass&&) = delete;
    SingletonClass& operator=(SingletonClass&&) = delete;
};
}  // namespace ashe
#endif  // !ASHE_SINGLETON_CLASS_H_
