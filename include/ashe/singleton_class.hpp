/*******************************************************************************
*    C++ Common Library
*    ---------------------------------------------------------------------------
*    Copyright (C) 2022 winsoft666 <winsoft666@outlook.com>.
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

#ifndef ASHE_SINGLETON_CLASS_HPP_
#define ASHE_SINGLETON_CLASS_HPP_
#include <mutex>

namespace ashe {
template <class T>
class SingletonClass {
   public:
    static T& Instance() {
        // Meyers' Singleton
        // If control enters the declaration concurrently while the variable is being initialized,
        // the concurrent execution shall wait for completion of the initialization.
        static T inst;
        return inst;
    }

   protected:
    SingletonClass() {}
    SingletonClass(const SingletonClass&) = delete;
    SingletonClass& operator=(const SingletonClass&) = delete;
    SingletonClass(SingletonClass&&) = delete;
    SingletonClass& operator=(SingletonClass&&) = delete;
};
}  // namespace ashe
#endif  // !ASHE_SINGLETON_CLASS_HPP_
