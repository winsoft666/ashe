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

#ifndef ASHE_WIN_ICON_HPP_
#define ASHE_WIN_ICON_HPP_
#pragma once

#include "ashe/config.hpp"
#include "ashe/arch.hpp"
#ifdef ASHE_WIN
#include <memory>
#include <string>
#include <vector>
#include "ashe/windows_lite.hpp"
#include "ashe/macros.hpp"

namespace ashe {
class ASHE_API WinIcon {
   public:
    class IconInfo {
        ASHE_DISALLOW_COPY(IconInfo);

       public:
        IconInfo() = default;

        inline IconInfo(IconInfo&& that) noexcept {
            *this = std::move(that);
        }

        inline IconInfo& operator=(IconInfo&& that) noexcept {
            if (this != &that) {
                this->hIcon = that.hIcon;
                that.hIcon = nullptr;

                this->cx = that.cx;
                that.cx = 0;

                this->cy = that.cy;
                that.cy = 0;
            }
            return *this;
        }

        inline ~IconInfo() {
            if (hIcon) {
                DestroyIcon(hIcon);
                hIcon = NULL;
            }
        }

        HICON hIcon = NULL;
        int cx = 0;
        int cy = 0;
    };

    struct IconGroup {
        std::wstring name;
        std::vector<std::shared_ptr<IconInfo>> icons;

        std::vector<HICON> iconHandleList() const noexcept;

        std::shared_ptr<IconInfo> getClosestSize(int desiredSize) noexcept;
    };

    // Load icon from resource of process.
    //  If cxDesired/cyDesired parameter is zero, the function uses the SM_CXICON/SM_CYICON.
    // You should call DestroyIcon for icon.
    static HICON LoadFromProcessRes(HINSTANCE hInst, LPCWSTR resPath, int cxDesired = 0, int cyDesired = 0) noexcept;

    // It is best to first use PathUtil::ExpandEnvString to expand environment variables in filePath.
    // You should call DestroyIcon for icon.
    static HICON LoadFromProcessRes(const std::wstring& filePath, LPCWSTR resPath, int cxDesired = 0, int cyDesired = 0) noexcept;

    // It is best to first use PathUtil::ExpandEnvString to expand environment variables in shell32Path.
    static bool ParseShell32IconInfo(const std::wstring& shell32Path,
                                     int shellIconIndex,
                                     std::wstring& iconPath,
                                     int& iconIndex) noexcept;

    // It is best to first use PathUtil::ExpandEnvString to expand environment variables in filePath.
    // Do not need call DestroyIcon for icon.
    static bool EnumIconGroups(const std::wstring& filePath, std::vector<IconGroup>& iconGroups) noexcept;

    // It is best to first use PathUtil::ExpandEnvString to expand environment variables in filePath.
    // You should call DestroyIcon for icon.
    static HICON LoadFromFile(const std::wstring& filePath, uint32_t iconIndex, int desiredSize = 256, int* actualSize = nullptr) noexcept;

    // The function uses the following steps to select the icon image:
    // Select the RT_GROUP_ICON resource. If more than one such resource exists,
    //    the system uses the first resource listed in the resource script.
    // Select the appropriate RT_ICON image from the RT_GROUP_ICON resource. If more than one image exists,
    //    the system uses the following criteria to choose an image:
    //        - The image closest in size to the requested size is chosen.
    //       - If two or more images of that size are present, the one that matches the color depth of the display is chosen.
    //       - If no images exactly match the color depth of the display, the image with the greatest color depth that
    //           does not exceed the color depth of the display is chosen.
    //           If all exceed the color depth, the one with the lowest color depth is chosen.
    //
    // It is best to first use PathUtil::ExpandEnvString to expand environment variables in filePath.
    // You should call DestroyIcon for icon.
    static HICON GetExeDisplayIcon(const std::wstring& filePath, int desiredSize, int* actualSize = nullptr) noexcept;

    // You should call DestroyIcon for icon.
    static HICON RunPrivateExtractIconsW(const std::wstring& filePath, int iconIndex, int desiredSize) noexcept;

    static bool SaveToFile(const std::vector<HICON>& hIcons, const std::wstring& filePath) noexcept;

   private:
    static bool DoEnumIconGroups(const std::wstring& filePath, bool onlyFirstGroup, std::vector<IconGroup>& iconGroups) noexcept;
};
}  // namespace ashe

#endif
#endif  // !ASHE_WIN_ICON_HPP_
