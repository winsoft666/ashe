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

#ifndef ASHE_WIN_INI_HPP_
#define ASHE_WIN_INI_HPP_
#pragma once

#include "ashe/config.hpp"
#include "ashe/arch.hpp"
#ifdef ASHE_WIN
#include <string>

namespace ashe {
class ASHE_API WinIni {
   public:
    WinIni() = default;

    WinIni(const std::wstring& file_path);

    WinIni(std::wstring&& file_path);

    ~WinIni() = default;

    void setIniFilePath(const std::wstring& file_path) noexcept;

    std::wstring iniFilePath() const noexcept;

    bool readInt(const std::wstring& item, const std::wstring& sub_item, unsigned int& result) noexcept;

    unsigned int readInt(const std::wstring& item,
                         const std::wstring& sub_item,
                         unsigned int default_value) noexcept;

    std::wstring readString(const std::wstring& item,
                            const std::wstring& sub_item,
                            const std::wstring& default_value) noexcept;

    bool readString(const std::wstring& item,
                    const std::wstring& sub_item,
                    std::wstring& result) noexcept;

    bool writeInt(const std::wstring& item, const std::wstring& sub_item, unsigned int value) noexcept;

    bool writeString(const std::wstring& item,
                     const std::wstring& sub_item,
                     const std::wstring& value) noexcept;

   protected:
    std::wstring ini_file_path_;
};
}  // namespace ashe
#endif  // !ASHE_WIN
#endif  // !ASHE_WIN_INI_HPP_
