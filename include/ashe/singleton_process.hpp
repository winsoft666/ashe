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
#ifndef ASHE_SINGLETON_PROCESS_HPP__
#define ASHE_SINGLETON_PROCESS_HPP__
#include "ashe/config.hpp"
#include "ashe/arch.hpp"
#include <string>
#include <future>
#include "ashe/event.hpp"
#include "ashe/singleton_class.hpp"

namespace ashe {
class ASHE_API SingletonProcess : public SingletonClass<SingletonProcess> {
   public:
    // when secondary process startup, SecondaryCallback will be call with the command line of secondary process.
    // The command line string is encoded in utf8.
    // SecondaryCallback is implemented only on windows platform.
    using SecondaryCallback = std::function<void(std::string secondaryCml)>;

    void markAndCheckStartup(const std::string& uniqueName);

    inline void registerSecondaryCallback(SecondaryCallback cb) { cb_ = cb; }

    const std::string& uniqueName() const;

    bool isPrimary() const;
#ifdef ASHE_WIN
    void* mutex() const;
#else
    int pidFile() const;
#endif
   protected:
    ~SingletonProcess();
    void check();

    std::string uniqueName_;
    bool isPrimary_ = true;
#ifdef ASHE_WIN
    void* mutex_ = NULL;
    void* secondaryStartupEvent_ = NULL;
    void* fileMapping_ = NULL;
    void* viewOfFile_ = NULL;
    std::future<void> asyncQuery_;
#else
    int pidFile_ = -1;
#endif
    SecondaryCallback cb_;
    ashe::Event exit_;

    friend class SingletonClass<SingletonProcess>;
};
}  // namespace ashe
#endif  // !ASHE_SINGLETON_PROCESS_HPP__
