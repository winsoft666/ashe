#ifndef ASHE_WIN_EXPLORER_UTIL_H_
#define ASHE_WIN_EXPLORER_UTIL_H_
#pragma once

#include "ashe/config.h"
#include <string>

namespace ashe {
namespace win {
ASHE_API bool OpenWinExplorerAndLocate(const std::wstring& path);

ASHE_API bool GetWinExplorerDisplayName(const std::wstring& path, std::wstring& displayName);
}  // namespace win
}  // namespace ashe

#endif  // !ASHE_WIN_EXPLORER_UTIL_H_
