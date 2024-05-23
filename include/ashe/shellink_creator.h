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

#ifdef ASHE_WIN
#include <string>
namespace ashe {
class ASHE_API ShellinkCreator {
   public:
    enum class OperationOption {
        // Create a new shortcut (overwriting if necessary).
        SHORTCUT_CREATE_ALWAYS = 0,
        // Overwrite an existing shortcut (fails if the shortcut doesn't exist).
        // If the arguments are not specified on the new shortcut, keep the old shortcut's arguments.
        SHORTCUT_REPLACE_EXISTING = 1,
        // Update specified properties only on an existing shortcut.
        SHORTCUT_UPDATE_EXISTING = 2,
    };

    struct ShellLinkProperties {
        enum IndividualProperties {
            PROPERTIES_TARGET = 1 << 0,
            PROPERTIES_WORKING_DIR = 1 << 1,
            PROPERTIES_ARGUMENTS = 1 << 2,
            PROPERTIES_DESCRIPTION = 1 << 3,
            PROPERTIES_ICON = 1 << 4,
        };

        ShellLinkProperties() :
            icon_index(-1), options(0U) {}

        void setTarget(const std::wstring& target_in) {
            target = target_in;
            options |= PROPERTIES_TARGET;
        }

        void setWorkingDir(const std::wstring& working_dir_in) {
            working_dir = working_dir_in;
            options |= PROPERTIES_WORKING_DIR;
        }

        void setArguments(const std::wstring& arguments_in) {
            // Size restriction as per MSDN at http://goo.gl/TJ7q5.
            arguments = arguments_in;
            options |= PROPERTIES_ARGUMENTS;
        }

        void setDescription(const std::wstring& description_in) {
            // Size restriction as per MSDN at http://goo.gl/OdNQq.
            description = description_in;
            options |= PROPERTIES_DESCRIPTION;
        }

        void setIcon(const std::wstring& icon_in, int icon_index_in) {
            icon = icon_in;
            icon_index = icon_index_in;
            options |= PROPERTIES_ICON;
        }

        // The target to launch from this shortcut. This is mandatory when creating a shortcut.
        std::wstring target;
        // The name of the working directory when launching the shortcut.
        std::wstring working_dir;
        // The arguments to be applied to |target| when launching from this shortcut.
        // The length of this string must be less than MAX_PATH.
        std::wstring arguments;
        // The localized description of the shortcut.
        // The length of this string must be less than MAX_PATH.
        std::wstring description;
        // The path to the icon (can be a dll or exe, in which case |icon_index| is the resource id).
        std::wstring icon;
        int icon_index;

        // Bitfield made of IndividualProperties. Properties set in |options| will be
        // set on the shortcut, others will be ignored.
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
}  // namespace ashe
#endif  // ASHE_WIN

#endif  // !ASHE_WIN_SHELLINK_CREATOR_HPP__