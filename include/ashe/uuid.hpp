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
#ifndef ASHE_GUID_HPP__
#define ASHE_GUID_HPP__
#pragma once

#include "ashe/config.hpp"
#include "ashe/arch.hpp"
#include <string>

namespace ashe {
class ASHE_API UUID {
   public:
    static std::string Create();

   private:
#ifndef ASHE_WIN
    static uint32_t Rand32();
    static std::string GenUuid4();
#endif
};
}  // namespace ashe
#endif  // !ASHE_GUID_HPP__
