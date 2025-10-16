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

#ifndef ASHE_WIN_MACHINE_UUID_H_
#define ASHE_WIN_MACHINE_UUID_H_
#pragma once
#include <string>
#include "ashe/config.h"

namespace ashe {
namespace win {
class ASHE_API MachineUUID {
   public:
    // 使用软件模拟唯一机器码
    // 从HKEY_CURRENT_USER\softwareKey\Value读取，如果不存在则创建一个新的UUID保存到该位置并返回
    //
    static std::wstring GetSoftwareUUID(const std::wstring& softwareKey);

    // 返回HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\MachineGuid的值
    // 通过Ghost等克隆软件克隆的系统，MachineGuid是相同的
    //
    static std::wstring GetMachineGUID();
};
}  // namespace win
}  // namespace ashe

#endif  // !ASHE_WIN_MACHINE_UUID_H_