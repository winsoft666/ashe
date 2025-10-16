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

#ifndef ASHE_WIN_ICON_UTIL_HPP_
#define ASHE_WIN_ICON_UTIL_HPP_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include <memory>
#include <string>
#include <vector>
#include "ashe/win/windows_lite.h"
#include "ashe/macros.h"

namespace ashe {
namespace win {
// 存储图标信息
//
class ASHE_API IconInfo {
   public:
    IconInfo() = default;

    inline IconInfo(IconInfo&& that) noexcept {
        swap(*this, that);
    }

    inline IconInfo& operator=(IconInfo&& that) noexcept {
        if (this != &that) {
            swap(*this, that);
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

   private:
    friend void swap(IconInfo& first, IconInfo& second) noexcept {
        using std::swap;
        swap(first.hIcon, second.hIcon);
        swap(first.cx, second.cx);
        swap(first.cy, second.cy);
    }
    ASHE_DISALLOW_COPY_AND_ASSIGN(IconInfo);
};

struct ASHE_API IconGroup {
    std::wstring name;
    std::vector<std::shared_ptr<IconInfo>> icons;

    std::vector<HICON> iconHandleList() const;

    std::shared_ptr<IconInfo> getClosestSize(int desiredSize);
};

// Load icon from resource of process.
//  If cxDesired/cyDesired parameter is zero, the function uses the SM_CXICON/SM_CYICON.
// You should call DestroyIcon for icon.
ASHE_API HICON LoadFromProcessRes(HINSTANCE hInst, LPCWSTR resPath, int cxDesired = 0, int cyDesired = 0);

// It is best to first use ExpandEnvString to expand environment variables in filePath.
// You should call DestroyIcon for icon.
ASHE_API HICON LoadFromProcessRes(const std::wstring& filePath, LPCWSTR resPath, int cxDesired = 0, int cyDesired = 0);

// It is best to first use ExpandEnvString to expand environment variables in shell32Path.
ASHE_API bool ParseShell32IconInfo(const std::wstring& shell32Path,
                                   int shellIconIndex,
                                   std::wstring& iconPath,
                                   int& iconIndex);

// It is best to first use ExpandEnvString to expand environment variables in filePath.
// Do not need call DestroyIcon for icon.
ASHE_API bool EnumIconGroups(const std::wstring& filePath, std::vector<IconGroup>& iconGroups);

// It is best to first use ExpandEnvString to expand environment variables in filePath.
// You should call DestroyIcon for icon.
ASHE_API HICON LoadFromFile(const std::wstring& filePath, uint32_t iconIndex, int desiredSize = 256, int* actualSize = nullptr);

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
// It is best to first use ExpandEnvString to expand environment variables in filePath.
// You should call DestroyIcon for icon.
ASHE_API HICON GetExeDisplayIcon(const std::wstring& filePath, int desiredSize, int* actualSize = nullptr);

// You should call DestroyIcon for icon.
ASHE_API HICON RunPrivateExtractIconsW(const std::wstring& filePath, int iconIndex, int desiredSize);

ASHE_API bool SaveToFile(const std::vector<HICON>& hIcons, const std::wstring& filePath);

// You should call DestroyIcon for icon.
ASHE_API HICON GetFileAssociatedIcon(const std::wstring& filePath,
                                     int iconIndex,
                                     int* gotIconIndex = NULL,
                                     std::wstring* associatedExePath = NULL);
}  // namespace win
}  // namespace ashe
#endif  // !ASHE_WIN_ICON_UTIL_HPP_
