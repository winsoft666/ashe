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

#ifndef ASHE_PATH_UTIL_HPP_
#define ASHE_PATH_UTIL_HPP_
#include "ashe/config.h"
#include <string>
#include <vector>
#include "ashe/arch.h"

namespace ashe {
// On Windows, the directory path ends with a "\"
class ASHE_API PathUtil {
   public:
#ifdef ASHE_WIN
    static std::wstring GetWindowsFolder() noexcept;

    static std::wstring GetSystemFolder() noexcept;

    static std::wstring GetTempFolder() noexcept;

    static std::wstring GetCurrentUserDesktopFolder() noexcept;

    static std::wstring GetAllUserDesktopFolder() noexcept;

    static std::wstring GetCurrentUserProgramsFolder() noexcept;
    static std::wstring GetAllUserProgramsFolder() noexcept;

    static std::wstring GetProgramFilesx86Folder() noexcept;
    static std::wstring GetProgramFilesFolder() noexcept;

    static std::wstring GetLocalAppDataFolder() noexcept;

    static bool OpenWinExplorerAndLocate(const std::wstring& path) noexcept;

    static std::wstring GetWinExplorerDisplayName(const std::wstring& path) noexcept;

    static std::string ReplaceKnownEnvToWow6432(const std::string& src);
    static std::wstring ReplaceKnownEnvToWow6432(const std::wstring& src);

    static std::string ExpandEnvString(const std::string& src, bool disableWow64FsRedirection = false) noexcept;
    static std::wstring ExpandEnvString(const std::wstring& src, bool disableWow64FsRedirection = false) noexcept;

#endif

    static bool PathIsSurroundQuote(const wchar_t* szPath) noexcept;
    static bool PathIsSurroundQuote(const std::wstring& path) noexcept;

    // Remove the quotes that surrounding the path.
    static void PathRemoveQuote(wchar_t* szPath) noexcept;
    static std::wstring PathRemoveQuote(const std::wstring& path) noexcept;

    static bool PathAddQuote(wchar_t* szPath, size_t buffSize, bool singleQuote = false) noexcept;
    static std::wstring PathAddQuote(const std::wstring& path, bool singleQuote = false) noexcept;
};
}  // namespace ashe
#endif  // !ASHE_PATH_UTIL_HPP_
