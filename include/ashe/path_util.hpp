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

#ifndef ASHE_PATH_UTIL_HPP_
#define ASHE_PATH_UTIL_HPP_
#include "ashe/config.hpp"
#include <string>
#include <vector>
#include "ashe/arch.hpp"

namespace ashe {
// On Windows, the directory path ends with a "\" 
class ASHE_API PathUtil {
   public:
#ifdef ASHE_WIN
    static std::wstring GetWindowsFolder();

    static std::wstring GetSystemFolder();

    static std::wstring GetTempFolder();

    static std::wstring GetCurrentUserDesktopFolder();

    static std::wstring GetAllUserDesktopFolder();

    static std::wstring GetCurrentUserProgramsFolder();
    static std::wstring GetAllUserProgramsFolder();

    static std::wstring GetProgramFilesx86Folder();
    static std::wstring GetProgramFilesFolder();

    static std::wstring GetLocalAppDataFolder();

    static bool OpenWinExplorerAndLocate(const std::wstring& path);

    static std::wstring GetWinExplorerDisplayName(const std::wstring& path);

    static std::string ReplaceKnownEnvToWow6432(const std::string& src);
    static std::wstring ReplaceKnownEnvToWow6432(const std::wstring& src);

    static std::string ExpandEnvString(const std::string& src, bool disableWow64FsRedirection = true);
    static std::wstring ExpandEnvString(const std::wstring& src, bool disableWow64FsRedirection = true);
#endif
};
}  // namespace ashe
#endif  // !ASHE_PATH_UTIL_HPP_
