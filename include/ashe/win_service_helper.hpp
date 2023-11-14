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

#ifndef ASHE_WIN_SERVICE_HELPER_HPP_
#define ASHE_WIN_SERVICE_HELPER_HPP_
#pragma once

#include "ashe/config.hpp"
#include "ashe/arch.hpp"
#ifdef ASHE_WIN

namespace ashe {
class ASHE_API WinServiceHelper {
   public:
    //
    //   FUNCTION: InstallService
    //
    //   PURPOSE: Install the current application as a service to the local
    //   service control manager database.
    //
    //   PARAMETERS:
    //   * pszServiceName - the name of the service to be installed
    //   * pszDisplayName - the display name of the service
    //   * dwStartType - the service start option. This parameter can be one of
    //     the following values: SERVICE_AUTO_START, SERVICE_BOOT_START,
    //     SERVICE_DEMAND_START, SERVICE_DISABLED, SERVICE_SYSTEM_START.
    //   * pszDependencies - a pointer to a double null-terminated array of null-
    //     separated names of services or load ordering groups that the system
    //     must start before this service.
    //   * pszTarget - Binary absolute path.
    //   * pszAccount - the name of the account under which the service runs.
    //   * pszPassword - the password to the account name.
    //
    //   NOTE: If the function fails to install the service, it prints the error
    //   in the standard output stream for users to diagnose the problem.
    //
    static bool Install(const wchar_t* pszServiceName,
                        const wchar_t* pszDisplayName,
                        const wchar_t* pszDecription,
                        unsigned int dwStartType,
                        const wchar_t* pszDependencies,
                        const wchar_t* pszTarget,
                        const wchar_t* pszAccount,
                        const wchar_t* pszPassword);

    //
    //   FUNCTION: UninstallService
    //
    //   PURPOSE: Stop and remove the service from the local service control
    //   manager database.
    //
    //   PARAMETERS:
    //   * pszServiceName - the name of the service to be removed.
    //
    //   NOTE: If the function fails to uninstall the service, it prints the
    //   error in the standard output stream for users to diagnose the problem.
    //
    static bool Uninstall(const wchar_t* pszServiceName);

    static bool IsInstall(const wchar_t* pszServiceName, bool &bInstalled);

    static bool Start(const wchar_t* pszServiceName);

    static bool Stop(const wchar_t* pszServiceName);

    static bool IsRunning(const wchar_t* pszServiceName, bool& bRunning);
};
}  // namespace ashe
#endif
#endif  // !ASHE_WIN_SERVICE_HELPER_HPP_