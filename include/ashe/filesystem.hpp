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
#ifndef ASHE_FILESYSTEM_HPP__
#define ASHE_FILESYSTEM_HPP__

//#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || (defined(__cplusplus) && __cplusplus >= 201703L)) && defined(__has_include)
//#if __has_include(<filesystem>) && (!defined(__MAC_OS_X_VERSION_MIN_REQUIRED) || __MAC_OS_X_VERSION_MIN_REQUIRED >= 101500)
//#define ASHE_USE_STD_FS
//#include <filesystem>
//namespace ashe {
//    namespace fs = std::filesystem;
//}
//#endif
//#endif


#include "external/gulrak-filesystem/include/ghc/filesystem.hpp"
namespace ashe {
namespace fs = ashe_ghc::filesystem;
}

#endif // !ASHE_FILESYSTEM_HPP__