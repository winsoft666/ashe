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

#ifndef ASHE_WIN_SHELLINK_CREATOR_HPP__
#define ASHE_WIN_SHELLINK_CREATOR_HPP__
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include <string>

namespace ashe {
namespace win {
// 创建Windows快捷方式.lnk文件
//
class ASHE_API ShellinkCreator {
   public:
    enum OperationOption {
        // 始终创建一个新的快捷方式，覆盖同名的已有快捷方式（如果存在的话）
        //
        SHORTCUT_CREATE_ALWAYS = 0,

        // 覆盖一个已有的快捷方式，如果不存在则失败
        // 如果新快捷方式没有指定参数（setArguments），则使用旧快捷方式的参数
        //
        SHORTCUT_REPLACE_EXISTING = 1,

        // 更新一个已有的快捷方式，如果不存在则失败
        //
        SHORTCUT_UPDATE_EXISTING = 2,
    };

    // 存储快捷方式的属性
    //
    struct ShellLinkProperties {
        // 不要直接使用这些枚举值，应该使用下面的setXXX()方法
        //
        enum IndividualProperties {
            PROPERTIES_TARGET = 1 << 0,
            PROPERTIES_WORKING_DIR = 1 << 1,
            PROPERTIES_ARGUMENTS = 1 << 2,
            PROPERTIES_DESCRIPTION = 1 << 3,
            PROPERTIES_ICON = 1 << 4,
        };

        ShellLinkProperties() :
            icon_index(-1), options(0U) {
        }

        // 快捷方式的目标
        void setTarget(const std::wstring& target_in) {
            target = target_in;
            options |= PROPERTIES_TARGET;
        }

        // 快捷方式的工作目录
        void setWorkingDir(const std::wstring& working_dir_in) {
            working_dir = working_dir_in;
            options |= PROPERTIES_WORKING_DIR;
        }

        // 快捷方式的参数
        void setArguments(const std::wstring& arguments_in) {
            // Size restriction as per MSDN at http://goo.gl/TJ7q5.
            arguments = arguments_in;
            options |= PROPERTIES_ARGUMENTS;
        }

        // 快捷方式的描述
        void setDescription(const std::wstring& description_in) {
            // Size restriction as per MSDN at http://goo.gl/OdNQq.
            description = description_in;
            options |= PROPERTIES_DESCRIPTION;
        }

        // 快捷方式的图标
        void setIcon(const std::wstring& icon_in, int icon_index_in) {
            icon = icon_in;
            icon_index = icon_index_in;
            options |= PROPERTIES_ICON;
        }

        std::wstring target;
        std::wstring working_dir;
        std::wstring arguments;
        std::wstring description;
        std::wstring icon;
        int icon_index;
        uint32_t options;
    };

    static bool CreateShellLink(const std::wstring& shellLinkPath,
                                const ShellLinkProperties& properties,
                                OperationOption operation);

    static bool ResolveShellLink(const std::wstring& shellLinkPath, ShellLinkProperties& properties);

    // Pins a shortcut to the Windows 7(or higher) taskbar.
    // The shortcut file must already exist and be a shortcut that points to an executable.
    static bool TaskbarPinShellLink(const std::wstring& shellLinkPath);

    // Unpins a shortcut from the Windows 7(or higher) taskbar.
    // The shortcut must exist and already be pinned to the taskbar.
    static bool TaskbarUnpinShellLink(const std::wstring& shellLinkPath);
};
}  // namespace win
}  // namespace ashe
#endif  // !ASHE_WIN_SHELLINK_CREATOR_HPP__