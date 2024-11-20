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
#ifndef ASHE_WIN_VIRTUAL_KEY_HPP__
#define ASHE_WIN_VIRTUAL_KEY_HPP__
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>
#include <unordered_map>

namespace ashe {
namespace win {
class ASHE_API VirtualKey {
   public:
    static std::string ToString(unsigned int vk);

    static unsigned int ToInteger(const std::string& s);

   private:
    static const std::unordered_map<std::string, unsigned int>& virtualKeyMaps();
};
}  // namespace win
}  // namespace ashe
#endif  // !ASHE_WIN_VIRTUAL_KEY_HPP__
