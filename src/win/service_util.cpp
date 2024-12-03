#include "ashe/config.h"
#include "ashe/win/service_util.h"
#ifndef _INC_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // !WIN32_LEAN_AND_MEAN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <Windows.h>
#endif
#include <stdio.h>
#include <strsafe.h>
#include "ashe/win/dbg_trace.h"

namespace ashe {
namespace win {
bool ServiceUtil::Install(const wchar_t* pszServiceName,
                               const wchar_t* pszDisplayName,
                               const wchar_t* pszDecription,
                               unsigned int dwStartType,
                               const wchar_t* pszDependencies,
                               const wchar_t* pszTarget,
                               const wchar_t* pszAccount,
                               const wchar_t* pszPassword) {
    bool result = false;
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;

    do {
        // Open the local default service control manager database
        schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE);
        if (schSCManager == NULL) {
            break;
        }

        // Install the service into SCM by calling CreateService
        schService =
            CreateServiceW(schSCManager,               // SCManager database
                           pszServiceName,             // Name of service
                           pszDisplayName,             // Name to display
                           SERVICE_ALL_ACCESS,         //SERVICE_QUERY_STATUS,           // Desired access
                           SERVICE_WIN32_OWN_PROCESS,  // Service type
                           dwStartType,                // Service start type
                           SERVICE_ERROR_NORMAL,       // Error control type
                           pszTarget,                  // Service's binary
                           NULL,                       // No load ordering group
                           NULL,                       // No tag identifier
                           NULL,                       //pszDependencies,                // Dependencies
                           NULL,                       //pszAccount,                     // Service running account
                           NULL                        //pszPassword                     // Password of the account
            );
        if (schService == NULL) {
            break;
        }

        // Set service description
        if (pszDecription) {
            wchar_t szDesc[MAX_PATH] = {0};
            StringCchCopyW(szDesc, MAX_PATH, pszDecription);

            SERVICE_DESCRIPTION ServiceDesc;
            ServiceDesc.lpDescription = szDesc;
            ::ChangeServiceConfig2W(schService, SERVICE_CONFIG_DESCRIPTION, &ServiceDesc);
        }

        result = true;
    } while (false);

    // Centralized cleanup for all allocated resources.
    if (schSCManager) {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService) {
        CloseServiceHandle(schService);
        schService = NULL;
    }

    return result;
}

bool ServiceUtil::Uninstall(const wchar_t* pszServiceName) {
    bool result = false;
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    do {
        // Open the local default service control manager database
        schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
        if (schSCManager == NULL) {
            break;
        }

        // Open the service with delete, stop, and query status permissions
        schService = OpenServiceW(schSCManager, pszServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS | DELETE);
        if (schService == NULL) {
            break;
        }

        // Try to stop the service
        if (ControlService(schService, SERVICE_CONTROL_STOP, &ssSvcStatus)) {
            Sleep(100);

            while (QueryServiceStatus(schService, &ssSvcStatus)) {
                if (ssSvcStatus.dwCurrentState == SERVICE_STOP_PENDING) {
                    Sleep(200);
                }
                else
                    break;
            }

            if (ssSvcStatus.dwCurrentState != SERVICE_STOPPED) {
                break;
            }
        }

        // Now remove the service by calling DeleteService.
        if (!DeleteService(schService)) {
            break;
        }

        result = true;
    } while (false);

    // Centralized cleanup for all allocated resources.
    if (schSCManager) {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService) {
        CloseServiceHandle(schService);
        schService = NULL;
    }

    return result;
}

bool ServiceUtil::IsInstall(const wchar_t* pszServiceName, bool& bInstalled) {
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        return false;
    }

    bool result = false;
    // Open the service with delete, stop, and query status permissions
    schService = OpenServiceW(schSCManager, pszServiceName, SERVICE_QUERY_STATUS);
    if (schService) {
        bInstalled = true;
        result = true;
    }
    else {
        if (GetLastError() == ERROR_SERVICE_DOES_NOT_EXIST) {
            bInstalled = false;
            result = true;
        }
    }

    if (schSCManager)
        CloseServiceHandle(schSCManager);

    if (schService)
        CloseServiceHandle(schService);

    return result;
}

bool ServiceUtil::QueryConfig(const wchar_t* pszServiceName, LPQUERY_SERVICE_CONFIGW* ppCfg) {
    if (!ppCfg) {
        return false;
    }

    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        return false;
    }

    bool result = false;
    // Open the service with delete, stop, and query status permissions
    schService = OpenServiceW(schSCManager, pszServiceName, SERVICE_QUERY_CONFIG);
    if (schService) {
        DWORD dwBufSize = 0;
        DWORD dwNeed = 0;
        if (!QueryServiceConfigW(schService, NULL, 0, &dwNeed)) {
            DWORD dwGLE = GetLastError();
            if (ERROR_INSUFFICIENT_BUFFER == dwGLE) {
                dwBufSize = dwNeed;
                *ppCfg = (LPQUERY_SERVICE_CONFIGW)LocalAlloc(LMEM_FIXED, dwBufSize);
                if (*ppCfg) {
                    dwNeed = 0;
                    if (QueryServiceConfigW(schService, *ppCfg, dwBufSize, &dwNeed)) {
                        result = true;
                    }
                    else {
                        LocalFree(*ppCfg);
                        *ppCfg = NULL;
                    }
                }
            }
        }
    }

    if (schSCManager)
        CloseServiceHandle(schSCManager);

    if (schService)
        CloseServiceHandle(schService);

    return result;
}

bool ServiceUtil::Start(const wchar_t* pszServiceName) {
    bool result = false;
    if (pszServiceName) {
        SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
        if (schSCManager) {
            SC_HANDLE schService = OpenServiceW(schSCManager, pszServiceName, SERVICE_START | SERVICE_QUERY_STATUS);
            if (schService) {
                SERVICE_STATUS status;
                if (QueryServiceStatus(schService, &status)) {
                    if (status.dwCurrentState == SERVICE_STOPPED) {
                        result = ::StartServiceW(schService, 0, NULL);
                    }
                }
                CloseServiceHandle(schService);
            }
            CloseServiceHandle(schSCManager);
        }
    }

    return result;
}

bool ServiceUtil::Stop(const wchar_t* pszServiceName) {
    bool result = false;
    if (pszServiceName) {
        SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, GENERIC_EXECUTE);
        if (schSCManager) {
            SC_HANDLE schService = OpenServiceW(schSCManager, pszServiceName, SERVICE_STOP | SERVICE_QUERY_STATUS);
            if (schService) {
                SERVICE_STATUS status;
                if (QueryServiceStatus(schService, &status)) {
                    if (status.dwCurrentState == SERVICE_RUNNING) {
                        result = ControlService(schService, SERVICE_CONTROL_STOP, &status);
                    }
                }
                CloseServiceHandle(schService);
            }
            CloseServiceHandle(schSCManager);
        }
    }

    return result;
}

bool ServiceUtil::IsRunning(const wchar_t* pszServiceName, bool& bRunning) {
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL) {
        return false;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenServiceW(schSCManager, pszServiceName, SERVICE_QUERY_STATUS);
    if (schService == NULL) {
        CloseServiceHandle(schSCManager);
        return false;
    }

    bool result = false;
    SERVICE_STATUS status;
    if (QueryServiceStatus(schService, &status)) {
        bRunning = status.dwCurrentState == SERVICE_RUNNING;
        result = true;
    }

    CloseServiceHandle(schSCManager);
    CloseServiceHandle(schService);

    return result;
}
}  // namespace win
}  // namespace ashe