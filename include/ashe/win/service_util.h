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

#ifndef ASHE_WIN_SERVICE_HELPER_HPP_
#define ASHE_WIN_SERVICE_HELPER_HPP_
#pragma once

#include "ashe/config.h"
#include "ashe/arch.h"
#include "ashe/win/windows_lite.h"

namespace ashe {
namespace win {
// Windows 服务的安装、卸载、启动、停止等操作的封装类
//
class ASHE_API ServiceUtil {
   public:
    //
    // 将pszTarget程序作为服务进行安装
    // 参数：
    // * pszServiceName - 要安装的服务名称
    // * pszDisplayName - 服务的显示名称
    // * dwStartType - 服务启动选项。此参数可以是以下值之一：
    //                SERVICE_AUTO_START（自动启动）、SERVICE_BOOT_START（启动时启动）、SERVICE_DEMAND_START（按需启动）、SERVICE_DISABLED（禁用）、SERVICE_SYSTEM_START（系统启动时启动）
    // * pszDependencies - 指向以两个 \0 终止的字符串，该字符串包含以 \0 分隔的服务名称或加载顺序组的名称，系统必须在启动此服务之前启动这些服务或组
    // * pszTarget - 服务程序二进制文件的路径
    // * pszAccount - 服务运行所使用的帐户名称
    // * pszPassword - 帐户名称的密码
    //
    static bool Install(const wchar_t* pszServiceName,
                        const wchar_t* pszDisplayName,
                        const wchar_t* pszDecription,
                        unsigned int dwStartType,
                        const wchar_t* pszDependencies,
                        const wchar_t* pszTarget,
                        const wchar_t* pszAccount,
                        const wchar_t* pszPassword);

    // 停止并卸载服务
    //
    static bool Uninstall(const wchar_t* pszServiceName);

    // 检测服务是否已安装
    //
    static bool IsInstall(const wchar_t* pszServiceName, bool& bInstalled);

    // Caller use LocalFree(*ppCfg) to feee memory if return true.
    // 查询服务配置（QUERY_SERVICE_CONFIGW）
    // QueryConfig会使用LocalAlloc来分配内存，调用者需要使用 LocalFree(*ppCfg) 来释放内存
    //
    static bool QueryConfig(const wchar_t* pszServiceName, LPQUERY_SERVICE_CONFIGW* ppCfg);

    // 启动服务
    //
    static bool Start(const wchar_t* pszServiceName);

    // 停止服务
    //
    static bool Stop(const wchar_t* pszServiceName);

    // 检测服务是否正在运行
    //
    static bool IsRunning(const wchar_t* pszServiceName, bool& bRunning);
};
}  // namespace win
}  // namespace ashe
#endif  // !ASHE_WIN_SERVICE_HELPER_HPP_