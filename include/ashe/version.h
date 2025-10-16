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

#ifndef ASHE_VERSION_HPP__
#define ASHE_VERSION_HPP__
#pragma once
#include "ashe/config.h"
#include <stdlib.h>
#include <string>
#include <vector>
#include "ashe/string_util.h"

namespace ashe {
// 版本号比较
// 支持 1.0.0, 1.0.0.0 格式，不支持 1.0.0-beta
//
class ASHE_API Version {
   public:
    Version(const std::string& s);

    Version(const std::wstring& s);

    Version(const Version& that) noexcept;

    bool isValid() const noexcept;

    bool isSameFormat(const Version& that) const noexcept;

    bool operator==(const Version& that) noexcept;

    bool operator!=(const Version& that) noexcept;

    bool operator>(const Version& that);

    bool operator<(const Version& that);

    bool operator<=(const Version& that);

    bool operator>=(const Version& that);

    Version& operator=(const Version& that) noexcept;

    int compare(const Version& that);

   protected:
    std::vector<unsigned int> verElems_;
};
}  // namespace ashe

#endif  // !ASHE_VERSION_HPP__