#include "ashe/config.h"
#include "ashe/process_util.h"

#ifdef ASHE_WIN
#include <tchar.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <TlHelp32.h>
#include <shellapi.h>
#include <Psapi.h>
#include "ashe/win/pe_util.h"
#include "ashe/os_version.h"
#else
#include <errno.h>
#include <signal.h>
#include <fstream>
#endif
#include "ashe/macros.h"
#include "ashe/string_encode.h"
#include "ashe/path_util.h"
#include "ashe/logging.h"

#pragma warning(disable : 4996)

namespace ashe {
#ifdef ASHE_WIN
bool EnablePrivilege(LPCTSTR szPrivilege, bool fEnable) {
    bool fOk = false;
    HANDLE hToken = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
        TOKEN_PRIVILEGES tp;
        tp.PrivilegeCount = 1;
        LookupPrivilegeValue(NULL, szPrivilege, &tp.Privileges[0].Luid);
        tp.Privileges->Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;

        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
        fOk = (GetLastError() == ERROR_SUCCESS);

        CloseHandle(hToken);
    }

    return fOk;
}

bool IsRunningAsElevation(HANDLE hProcess) {
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

bool IsRunningAsElevation(DWORD dwPid) {
    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
    if (!hProcess)
        return false;
    const bool ret = IsRunningAsElevation(hProcess);
    CloseHandle(hProcess);
    return ret;
}

bool SetUIPIMsgFilter(HWND hWnd, unsigned int uMessageID, bool bAllow) {
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

bool CreateNewProcess(const std::wstring& path, const std::wstring& param, DWORD* dwPID, HANDLE* pProcess) {
    std::wstring newPath = PathRemoveQuote(path);

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

bool RunAsAdmin(const std::wstring& path, const std::wstring& param, int nShowCmd /*= SW_SHOWDEFAULT*/) {
    std::wstring newPath = PathRemoveQuote(path);
    WCHAR szDir[MAX_PATH] = {0};
    StringCchPrintfW(szDir, MAX_PATH, L"%s", newPath.c_str());
    PathRemoveFileSpecW(szDir);

    bool result = (INT_PTR)(::ShellExecuteW(
                      nullptr,
                      L"runas",
                      path.c_str(),
                      param.c_str(),
                      szDir,
                      nShowCmd)) > 32;
    return result;
}

bool Is32BitProcess(HANDLE process) {
    if (!process)
        return false;

    bool wow64 = IsWow64Process(process);
    if (wow64)
        return true;

    return !IsWin64();
}

bool Is32BitProcess(unsigned long pid) {
    bool result = false;
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    DCHECK(process) << "Unable to open process with PROCESS_QUERY_INFORMATION, pid:" << pid;
    if (process) {
        result = Is32BitProcess(process);
        CloseHandle(process);
    }
    return result;
}

bool GetCurrentExePath(std::wstring& path) {
    bool result = false;
    wchar_t* pBuf = NULL;
    DWORD dwBufSize = MAX_PATH;

    try {
        path.clear();

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
                pBuf = NULL;
                dwBufSize *= 2;
            }
            else {
                result = true;
                break;
            }
        } while (true);

        if (result) {
            path = pBuf;
        }

        if (pBuf) {
            free(pBuf);
            pBuf = NULL;
        }
    } catch (std::exception& e) {
        DLOG(LS_FATAL) << "exception occurred: " << e.what();
        return false;
    }

    return result;
}

bool GetCurrentExePath(std::string& path) {
    std::wstring pathW;
    if (!GetCurrentExePath(pathW)) {
        NOTREACHED();
        return false;
    }

    path = w2a(pathW);
    return true;
}

std::wstring GetCurrentExePathW() {
    std::wstring path;
    if (!GetCurrentExePath(path)) {
        NOTREACHED();
        return std::wstring();
    }
    return path;
}

std::string GetCurrentExePathA() {
    return w2a(GetCurrentExePathW());
}

bool GetCurrentExeDirectory(std::wstring& dir) {
    std::wstring path;
    if (!GetCurrentExePath(path))
        return false;

    dir = PathGetDirectory(path, 1);
    return true;
}

bool GetCurrentExeDirectory(std::string& dir) {
    std::wstring dirW;
    if (!GetCurrentExeDirectory(dirW)) {
        NOTREACHED();
        return false;
    }

    dir = w2a(dirW);
    return true;
}

std::wstring GetCurrentExeDirectoryW() {
    std::wstring dir;
    if (!GetCurrentExeDirectory(dir)) {
        NOTREACHED();
        return std::wstring();
    }
    return dir;
}

std::string GetCurrentExeDirectoryA() {
    return w2a(GetCurrentExeDirectoryW());
}

bool IsWow64Process(HANDLE process) {
    BOOL bIsWow64 = FALSE;

    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    HMODULE hDll = GetModuleHandle(TEXT("kernel32"));
    if (hDll == NULL) {
        NOTREACHED();
        return false;
    }

    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hDll, "IsWow64Process");
    if (fnIsWow64Process == NULL) {
        NOTREACHED();
        return false;
    }

    if (!fnIsWow64Process(process, &bIsWow64)) {
        NOTREACHED();
        return false;
    }

    return !!bIsWow64;
}

bool IsWow64Process(unsigned long pid) {
    bool result = false;
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    DCHECK(process) << "Unable to open process with PROCESS_QUERY_INFORMATION, pid:" << pid;
    if (process) {
        result = IsWow64Process(process);
        CloseHandle(process);
    }
    return result;
}

bool Is64BitProcess(unsigned long pid) {
    return IsWin64() && !IsWow64Process(pid);
}

bool Is64BitProcess(HANDLE process) {
    return IsWin64() && !IsWow64Process(process);
}

bool IsPeX64(LPCWSTR pszModulePath) {
    HMODULE h = GetModuleHandleW(pszModulePath);
    DCHECK(h) << "the module must have been loaded by the calling process.";
    if (!h)
        return false;
    return PE_OPT_HEADER((CHAR*)h)->Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC;
}

BOOL CALLBACK EnumResourceNameCallback(HMODULE hModule, LPCWSTR lpType, LPWSTR lpName, LONG_PTR lParam) {
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

bool GetExeOrDllManifest(const std::wstring& path, std::list<std::string>& manifests) {
    HMODULE hModule = LoadLibraryExW(path.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (!hModule)
        return false;

    EnumResourceNames(hModule, RT_MANIFEST, EnumResourceNameCallback, (LONG_PTR)&manifests);
    FreeLibrary(hModule);

    return true;
}

// Based on http://stackoverflow.com/a/1173396
bool KillProcessTree(unsigned long pid, unsigned int exitCode) {
    if (pid == 0)
        return false;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    bool allSuccess = true;
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);
    if (Process32First(snapshot, &process)) {
        do {
            if (process.th32ParentProcessID == pid) {
                const HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
                if (h) {
                    if (!TerminateProcess(h, exitCode))
                        allSuccess = false;
                    CloseHandle(h);
                }
                else {
                    allSuccess = false;
                }
            }
        } while (Process32Next(snapshot, &process));
    }
    CloseHandle(snapshot);

    const HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (h) {
        if (!TerminateProcess(h, exitCode))
            allSuccess = false;
        CloseHandle(h);
    }
    else {
        allSuccess = false;
    }

    return allSuccess;
}

bool KillProcess(unsigned long pid, unsigned int exitCode) {
    if (pid == 0)
        return false;

    const HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!h)
        return false;
    return !!TerminateProcess(h, exitCode);
}

bool KillProcess(const std::wstring& exeName, unsigned int exitCode) {
    if (exeName.empty())
        return false;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;

    bool allSuccess = true;
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);
    if (Process32First(snapshot, &process)) {
        do {
            if (lstrcmpi(process.szExeFile, exeName.c_str()) == 0) {
                HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
                if (h) {
                    if (!::TerminateProcess(h, exitCode)) {
                        allSuccess = false;
                    }

                    CloseHandle(h);
                }
                else {
                    allSuccess = false;
                }
            }
        } while (Process32Next(snapshot, &process));
    }
    CloseHandle(snapshot);

    return allSuccess;
}

bool KillProcess(const std::string& exeName, unsigned int exitCode) {
    return KillProcess(a2w(exeName), exitCode);
}

std::wstring GetProcessPathW(unsigned long id) {
    std::wstring strPath;
    wchar_t szFilename[MAX_PATH] = {0};
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, id);
    if (hProcess == NULL)
        return strPath;

    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameW(hProcess, 0, szFilename, &size)) {
        strPath = szFilename;
    }

    CloseHandle(hProcess);

    return strPath;
}

std::string GetProcoessPathA(unsigned long id) {
    std::string strPath;
    char szFilename[MAX_PATH] = {0};
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, id);
    if (hProcess == NULL)
        return strPath;

    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameA(hProcess, 0, szFilename, &size)) {
        strPath = szFilename;
    }

    CloseHandle(hProcess);

    return strPath;
}
#else
void KillProcessTree(pid_t id, bool force) {
    if (id <= 0)
        return;

    if (force)
        ::kill(-id, SIGTERM);
    else
        ::kill(-id, SIGINT);
}

bool KillProcess(pid_t id, bool force) {
    if (force)
        return ::kill(id, SIGTERM) == 0;
    return ::kill(id, SIGINT) == 0;
}

std::string GetProcessPathA(pid_t id) {
    std::string cmdLine;
    try {
        std::string cmdPath = std::string("/proc/") + std::to_string(id) + std::string("/cmdline");
        std::ifstream cmdFile(cmdPath.c_str());

        std::getline(cmdFile, cmdLine);
        if (!cmdLine.empty()) {
            // Keep first cmdline item which contains the program path
            size_t pos = cmdLine.find('\0');
            if (pos != std::string::npos)
                cmdLine = cmdLine.substr(0, pos);
        }
    } catch (std::exception& e) {
        DLOG(LS_FATAL) << "exception occurred: " << e.what();
    }
    return cmdLine;
}

std::wstring GetProcessPathW(pid_t id) {
    return u2w(GetProcessPathA(id));
}
#endif

}  // namespace ashe