#include "ashe/config.h"
#include "ashe/process_util.h"

#ifdef ASHE_WIN
#include <tchar.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <shellapi.h>
#elif defined(_GNU_SOURCE)
#include <errno.h>
#endif
#include "ashe/macros.h"
#include "ashe/string_encode.h"
#include "ashe/path_util.h"
#include "ashe/os_ver.h"

#pragma warning(disable : 4996)

namespace ashe {
#ifdef ASHE_WIN
bool ProcessUtil::IsRunAsAdminPrivilege(HANDLE hProcess) noexcept {
    BOOL fRet = FALSE;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION Elevation;
        DWORD cbSize = sizeof(TOKEN_ELEVATION);
        if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
            fRet = (Elevation.TokenIsElevated != 0);
        }
    }

    if (hToken) {
        CloseHandle(hToken);
    }

    return !!fRet;
}

bool ProcessUtil::IsRunAsAdminPrivilege(DWORD dwPid) noexcept {
    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
    if (!hProcess)
        return false;
    const bool ret = IsRunAsAdminPrivilege(hProcess);
    CloseHandle(hProcess);
    return ret;
}

bool ProcessUtil::SetUIPIMsgFilter(HWND hWnd, unsigned int uMessageID, bool bAllow) noexcept {
    OSVERSIONINFO VersionTmp;
    VersionTmp.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&VersionTmp);
    BOOL res = FALSE;

    if (VersionTmp.dwMajorVersion >= 6) {  // vista above.
        BOOL(WINAPI * pfnChangeMessageFilterEx)
        (HWND, UINT, DWORD, PCHANGEFILTERSTRUCT);
        BOOL(WINAPI * pfnChangeMessageFilter)
        (UINT, DWORD);

        CHANGEFILTERSTRUCT filterStatus;
        filterStatus.cbSize = sizeof(CHANGEFILTERSTRUCT);

        HINSTANCE hlib = LoadLibrary(_T("user32.dll"));
        if (hlib) {
            (FARPROC&)pfnChangeMessageFilterEx = GetProcAddress(hlib, "ChangeWindowMessageFilterEx");

            if (pfnChangeMessageFilterEx != NULL && hWnd != NULL) {
                res = pfnChangeMessageFilterEx(hWnd, uMessageID, (bAllow ? MSGFLT_ADD : MSGFLT_REMOVE),
                                               &filterStatus);
            }

            // If failed, try again.
            if (!res) {
                (FARPROC&)pfnChangeMessageFilter = GetProcAddress(hlib, "ChangeWindowMessageFilter");

                if (pfnChangeMessageFilter != NULL) {
                    res = pfnChangeMessageFilter(uMessageID, (bAllow ? MSGFLT_ADD : MSGFLT_REMOVE));
                }
            }

            FreeLibrary(hlib);
            hlib = NULL;
        }
    }
    else {
        res = TRUE;
    }

    return !!res;
}

bool ProcessUtil::CreateNewProcess(const std::wstring& path, const std::wstring& param, DWORD* dwPID, HANDLE* pProcess) {
    std::wstring newPath = PathUtil::PathRemoveQuote(path);

    WCHAR szDir[MAX_PATH] = {0};
    StringCchPrintfW(szDir, MAX_PATH, L"%s", newPath.c_str());
    PathRemoveFileSpecW(szDir);

    WCHAR szFullCMD[1024];
    if (param.length() > 0)
        StringCchPrintfW(szFullCMD, 1024, L"\"%s\" %s", newPath.c_str(), param.c_str());
    else
        StringCchPrintfW(szFullCMD, 1024, L"\"%s\"", newPath.c_str());

    STARTUPINFOW si = {sizeof(si)};
    PROCESS_INFORMATION pi = {0};
    if (!CreateProcessW(NULL, szFullCMD, NULL, NULL, FALSE, 0, NULL, szDir, &si, &pi))
        return false;

    if (pi.hThread)
        CloseHandle(pi.hThread);

    if (pProcess) {
        *pProcess = pi.hProcess;
    }
    else {
        SAFE_CLOSE(pi.hProcess);
    }

    if (dwPID) {
        *dwPID = pi.dwProcessId;
    }

    return true;
}

bool ProcessUtil::RunAsAdmin(const std::wstring& path, const std::wstring& param, int nShowCmd /*= SW_SHOWDEFAULT*/) {
    std::wstring newPath = PathUtil::PathRemoveQuote(path);
    WCHAR szDir[MAX_PATH] = {0};
    StringCchPrintfW(szDir, MAX_PATH, L"%s", newPath.c_str());
    PathRemoveFileSpecW(szDir);

    bool result = (INT_PTR)(::ShellExecuteW(
                      nullptr,
                      L"runas",
                      path.c_str(),
                      param.c_str(),
                      szDir,
                      nShowCmd)) > 31;
    return result;
}

bool ProcessUtil::Is32BitProcess(HANDLE process, bool& result) noexcept {
    if (!process)
        return false;

    bool wow64 = false;
    if (!OSVersion::IsWow64(process, wow64))
        return false;

    if (wow64)
        result = true;
    else
        result = !OSVersion::IsWin64();
    return true;
}

std::wstring ProcessUtil::GetCurrentExePath() {
    wchar_t* buf = nullptr;
    if (!GetCurrentExePath(&buf)) {
        return L"";
    }

    std::wstring result = buf;
    free(buf);

    return result;
}

bool ProcessUtil::GetCurrentExePath(wchar_t** buf) {
    if (!buf) {
        return false;
    }

    bool result = false;
    wchar_t* pBuf = NULL;
    DWORD dwBufSize = MAX_PATH;

    do {
        pBuf = (wchar_t*)malloc((dwBufSize + 1) * sizeof(wchar_t));
        if (!pBuf)
            break;
        memset(pBuf, 0, (dwBufSize + 1) * sizeof(wchar_t));

        DWORD dwGot = GetModuleFileNameW(NULL, pBuf, dwBufSize);
        if (dwGot == 0) {
            break;
        }

        if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
            free(pBuf);
            dwBufSize *= 2;
        }
        else {
            result = true;
            break;
        }
    } while (true);

    if (result) {
        *buf = pBuf;
    }
    else {
        if (pBuf) {
            free(pBuf);
        }
    }

    return result;
}

std::wstring ProcessUtil::GetCurrentExeDirectory() {
    wchar_t* buf = nullptr;
    if (!GetCurrentExePath(&buf)) {
        return L"";
    }

    if (!PathRemoveFileSpecW(buf)) {
        free(buf);
        return L"";
    }

    std::wstring result = buf;
    free(buf);

    return result;
}

#endif

#ifdef ASHE_WIN
BOOL CALLBACK ProcessUtil::EnumResourceNameCallback(HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam) {
    std::list<std::string>* pList = (std::list<std::string>*)lParam;

    HRSRC hResInfo = FindResourceW(hModule, lpName, lpType);
    DWORD cbResource = SizeofResource(hModule, hResInfo);

    HGLOBAL hResData = LoadResource(hModule, hResInfo);
    if (hResData) {
        const BYTE* pResource = (const BYTE*)LockResource(hResData);

        if (pResource) {
            std::string strU8;
            strU8.assign((const char*)pResource, cbResource);
            pList->push_back(strU8);
        }

        UnlockResource(hResData);
        FreeResource(hResData);
    }

    return TRUE;  // Keep going
}

bool ProcessUtil::GetExeOrDllManifest(const std::wstring& path, std::list<std::string>& manifests) {
    HMODULE hModule = LoadLibraryExW(path.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (!hModule)
        return false;

    EnumResourceNames(hModule, RT_MANIFEST, EnumResourceNameCallback, (LONG_PTR)&manifests);
    FreeLibrary(hModule);

    return true;
}
#endif
}  // namespace ashe