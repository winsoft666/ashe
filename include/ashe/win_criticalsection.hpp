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

#ifndef ASHE_WIN_CRITICAL_SECTION_HPP__
#define ASHE_WIN_CRITICAL_SECTION_HPP__
#pragma once
//
// Windows CriticalSection wrapper.
//
#include "ashe/config.hpp"
#include "ashe/arch.hpp"
#ifdef ASHE_WIN
#include "ashe/macros.hpp"

namespace ashe {
class ASHE_API WinCriticalSection {
   public:
    ASHE_DISALLOW_COPY_MOVE(WinCriticalSection);

    WinCriticalSection();
    ~WinCriticalSection();
    void enter() const;
    void leave() const;
    bool tryEnter() const;

   private:
    class Private;
    Private* p_ = nullptr;
};

class ASHE_API ScopedWinCriticalSection {
   public:
    ASHE_DISALLOW_COPY_MOVE(ScopedWinCriticalSection);

    explicit ScopedWinCriticalSection(const WinCriticalSection* pCS) :
        crit_(pCS) { crit_->enter(); }
    ~ScopedWinCriticalSection() { crit_->leave(); }

   private:
    const WinCriticalSection* const crit_;
};
}  // namespace ashe

#endif  // !ASHE_WIN
#endif  // !ASHE_WIN_CRITICAL_SECTION_HPP__
