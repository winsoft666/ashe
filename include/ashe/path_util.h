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
#ifdef ASHE_WIN
ASHE_API std::wstring GetWindowsFolder() noexcept;

ASHE_API std::wstring GetSystemFolder() noexcept;

ASHE_API std::wstring GetTempFolder() noexcept;

ASHE_API std::wstring GetCurrentUserDesktopFolder() noexcept;

ASHE_API std::wstring GetAllUserDesktopFolder() noexcept;

ASHE_API std::wstring GetCurrentUserProgramsFolder() noexcept;
ASHE_API std::wstring GetAllUserProgramsFolder() noexcept;

ASHE_API std::wstring GetProgramFilesx86Folder() noexcept;
ASHE_API std::wstring GetProgramFilesFolder() noexcept;

ASHE_API std::wstring GetLocalAppDataFolder() noexcept;

ASHE_API bool OpenWinExplorerAndLocate(const std::wstring& path) noexcept;

ASHE_API std::wstring GetWinExplorerDisplayName(const std::wstring& path) noexcept;

ASHE_API std::string ReplaceKnownEnvToWow6432(const std::string& src);
ASHE_API std::wstring ReplaceKnownEnvToWow6432(const std::wstring& src);

ASHE_API std::string ExpandEnvString(const std::string& src, bool disableWow64FsRedirection = false) noexcept;
ASHE_API std::wstring ExpandEnvString(const std::wstring& src, bool disableWow64FsRedirection = false) noexcept;

#endif

ASHE_API bool PathIsSurroundQuote(const wchar_t* szPath) noexcept;
ASHE_API bool PathIsSurroundQuote(const std::wstring& path) noexcept;

// Remove the quotes that surrounding the path.
ASHE_API void PathRemoveQuote(wchar_t* szPath) noexcept;
ASHE_API std::wstring PathRemoveQuote(const std::wstring& path) noexcept;

ASHE_API bool PathAddQuote(wchar_t* szPath, size_t buffSize, bool singleQuote = false) noexcept;
ASHE_API std::wstring PathAddQuote(const std::wstring& path, bool singleQuote = false) noexcept;

ASHE_API std::wstring GetDirFromPath(const std::wstring& path, int upLevel = 1) noexcept;

ASHE_API std::wstring GetFileNameFromPath(const std::wstring& path) noexcept;

ASHE_API std::wstring CombinePath(const std::wstring& path1, const std::wstring& path2) noexcept;

ASHE_API bool IsExistedPath(const std::wstring& path) noexcept;

ASHE_API bool IsFilePath(const std::wstring& path) noexcept;

ASHE_API bool IsDirPath(const std::wstring& path) noexcept;
}  // namespace ashe
#endif  // !ASHE_PATH_UTIL_HPP_
