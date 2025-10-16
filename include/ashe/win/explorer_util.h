#ifndef ASHE_WIN_EXPLORER_UTIL_H_
#define ASHE_WIN_EXPLORER_UTIL_H_
#pragma once

#include "ashe/config.h"
#include <string>

namespace ashe {
namespace win {
// 在 Windows 文件资源管理器中打开 path 所在父目录，并自动选中 path 所指定的文件/目录
//
ASHE_API bool OpenWinExplorerAndLocate(const std::wstring& path);

// 获取 path 在 Windows 文件资源管理器中的显示名称，如 C:\Users\xxx\Documents 通常显示为 "文档"
//
ASHE_API bool GetWinExplorerDisplayName(const std::wstring& path, std::wstring& displayName);
}  // namespace win
}  // namespace ashe

#endif  // !ASHE_WIN_EXPLORER_UTIL_H_
