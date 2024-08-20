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
ASHE_API std::wstring GetWindowsFolder();

ASHE_API std::wstring GetSystemFolder();

ASHE_API std::wstring GetTempFolder();

ASHE_API std::wstring GetCurrentUserDesktopFolder();

ASHE_API std::wstring GetAllUserDesktopFolder();

ASHE_API std::wstring GetCurrentUserProgramsFolder();
ASHE_API std::wstring GetAllUserProgramsFolder();

ASHE_API std::wstring GetProgramFilesx86Folder();
ASHE_API std::wstring GetProgramFilesFolder();

ASHE_API std::wstring GetLocalAppDataFolder();

ASHE_API bool OpenWinExplorerAndLocate(const std::wstring& path);

ASHE_API std::wstring GetWinExplorerDisplayName(const std::wstring& path);

ASHE_API std::string ReplaceKnownEnvToWow6432(const std::string& src);
ASHE_API std::wstring ReplaceKnownEnvToWow6432(const std::wstring& src);

ASHE_API std::string ExpandEnvString(const std::string& src, bool disableWow64FsRedirection = false);
ASHE_API std::wstring ExpandEnvString(const std::wstring& src, bool disableWow64FsRedirection = false);

#endif

ASHE_API bool PathIsSurroundQuote(const wchar_t* szPath);
ASHE_API bool PathIsSurroundQuote(const std::wstring& path);

// Remove the quotes that surrounding the path.
ASHE_API void PathRemoveQuote(wchar_t* szPath);
ASHE_API std::wstring PathRemoveQuote(const std::wstring& path);

ASHE_API bool PathAddQuote(wchar_t* szPath, size_t buffSize, bool singleQuote = false);
ASHE_API std::wstring PathAddQuote(const std::wstring& path, bool singleQuote = false);

ASHE_API std::wstring PathGetDirectory(const std::wstring& path, int upLevel = 1);

ASHE_API std::wstring PathGetFileName(const std::wstring& path);

ASHE_API std::wstring PathJoin(const std::wstring& path1, const std::wstring& path2);

ASHE_API bool PathIsExist(const std::wstring& path);

ASHE_API bool PathIsFile(const std::wstring& path);

ASHE_API bool PathIsDirectory(const std::wstring& path);
}  // namespace ashe
#endif  // !ASHE_PATH_UTIL_HPP_
