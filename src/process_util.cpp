#include "ashe/config.h"
#include "ashe/process_util.h"

#ifdef ASHE_WIN
#include <tchar.h>
#include <strsafe.h>
#include <Shlwapi.h>
#include <TlHelp32.h>
#include <shellapi.h>
#include <Psapi.h>
#include "ashe/win_pe.h"
#elif defined(_GNU_SOURCE)
#include <errno.h>
#endif
#include "ashe/macros.h"
#include "ashe/string_encode.h"
#include "ashe/path_util.h"
#include "ashe/os_version.h"
#include "ashe/check_failure.h"

#pragma warning(disable : 4996)

namespace ashe {
#ifdef ASHE_WIN
bool IsRunAsAdminPrivilege(HANDLE hProcess) noexcept {
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

bool IsRunAsAdminPrivilege(DWORD dwPid) noexcept {
    HANDLE hProcess = ::OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPid);
    if (!hProcess)
        return false;
    const bool ret = IsRunAsAdminPrivilege(hProcess);
    CloseHandle(hProcess);
    return ret;
}

bool SetUIPIMsgFilter(HWND hWnd, unsigned int uMessageID, bool bAllow) noexcept {
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
                      nShowCmd)) > 31;
    return result;
}

bool Is32BitProcess(HANDLE process) noexcept {
    if (!process)
        return false;

    bool wow64 = IsWow64Process(process);
    if (wow64)
        return true;

    return !IsWin64();
}

std::wstring GetCurrentExePathW() {
    wchar_t* buf = nullptr;
    if (!GetCurrentExePath(&buf)) {
        return L"";
    }

    std::wstring result = buf;
    free(buf);

    return result;
}

std::string GetCurrentExePathA() {
    char* buf = nullptr;
    if (!GetCurrentExePath(&buf)) {
        return "";
    }

    std::string result = buf;
    free(buf);

    return result;
}

bool GetCurrentExePath(wchar_t** buf) {
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

bool GetCurrentExePath(char** buf) {
    if (!buf) {
        return false;
    }

    bool result = false;
    char* pBuf = NULL;
    DWORD dwBufSize = MAX_PATH;

    do {
        pBuf = (char*)malloc((dwBufSize + 1) * sizeof(char));
        if (!pBuf)
            break;
        memset(pBuf, 0, (dwBufSize + 1) * sizeof(char));

        DWORD dwGot = GetModuleFileNameA(NULL, pBuf, dwBufSize);
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

std::wstring GetCurrentExeDirectoryW() {
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

std::string GetCurrentExeDirectoryA() {
    char* buf = nullptr;
    if (!GetCurrentExePath(&buf)) {
        return "";
    }

    if (!PathRemoveFileSpecA(buf)) {
        free(buf);
        return "";
    }

    std::string result = buf;
    free(buf);

    return result;
}

bool IsWow64Process(HANDLE process) noexcept {
    BOOL bIsWow64 = FALSE;

    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    HMODULE hDll = GetModuleHandle(TEXT("kernel32"));
    if (ASHE_CHECK_FAILURE(hDll != NULL, L"get kernel32 handle failed"))
        return false;

    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(hDll, "IsWow64Process");
    if (ASHE_CHECK_FAILURE(fnIsWow64Process != NULL, L"get IsWow64Process address failed"))
        return false;

    if (ASHE_CHECK_FAILURE(!!fnIsWow64Process(process, &bIsWow64), L"IsWow64Process failed"))
        return false;

    return !!bIsWow64;
}

bool IsWow64Process(unsigned long pid) noexcept {
    bool result = false;
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (process) {
        result = IsWow64Process(process);
        CloseHandle(process);
    }
    return result;
}

bool IsX64Process(unsigned long pid) {
    return IsWin64() && !IsWow64Process(pid);
}

bool IsPeX64(LPCWSTR pszModulePath) {
    HMODULE h = GetModuleHandleW(pszModulePath);
    if (ASHE_CHECK_FAILURE(h, L"the module must have been loaded by the calling process."))
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
void KillProcessTree(unsigned long pid) noexcept {
    if (pid == 0)
        return;

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot) {
        PROCESSENTRY32 process;
        ZeroMemory(&process, sizeof(process));
        process.dwSize = sizeof(process);
        if (Process32First(snapshot, &process)) {
            do {
                if (process.th32ParentProcessID == pid) {
                    const HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
                    if (process_handle) {
                        TerminateProcess(process_handle, 2);
                        CloseHandle(process_handle);
                    }
                }
            } while (Process32Next(snapshot, &process));
        }
        CloseHandle(snapshot);
    }

    const HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (process_handle)
        TerminateProcess(process_handle, 2);
}

bool KillProcess(unsigned long pid) noexcept {
    if (pid == 0)
        return false;

    const HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (!process_handle)
        return false;
    return !!TerminateProcess(process_handle, 2);
}

bool KillProcess(const std::wstring& exeName) noexcept {
    if (exeName.empty())
        return false;

    bool ret = false;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot) {
        PROCESSENTRY32 process;
        ZeroMemory(&process, sizeof(process));
        process.dwSize = sizeof(process);
        if (Process32First(snapshot, &process)) {
            do {
                if (lstrcmpi(process.szExeFile, exeName.c_str()) == 0) {
                    HANDLE process_handle = OpenProcess(PROCESS_TERMINATE, FALSE, process.th32ProcessID);
                    if (process_handle) {
                        if (::TerminateProcess(process_handle, 2))
                            ret = true;
                        CloseHandle(process_handle);
                    }
                }
            } while (Process32Next(snapshot, &process));
        }
        CloseHandle(snapshot);
    }
    return ret;
}

bool KillProcess(const std::string& exeName) noexcept {
    return KillProcess(a2w(exeName));
}

void RecursiveKillProcess(const std::wstring& dir, bool excludeSelf) noexcept {
    size_t len = dir.length();
    TCHAR szTemp[MAX_PATH] = {0};

    StringCchCopy(szTemp, MAX_PATH, dir.c_str());

    if (szTemp[len - 1] != TEXT('\\'))
        _tcscat_s(szTemp, MAX_PATH, TEXT("\\"));

    StringCchCat(szTemp, MAX_PATH, TEXT("*.*"));

    WIN32_FIND_DATA filedata;
    HANDLE fhandle = FindFirstFile(szTemp, &filedata);

    if (fhandle != INVALID_HANDLE_VALUE) {
        if (filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (((_tcscmp(filedata.cFileName, TEXT(".")) != 0) &&
                 (_tcscmp(filedata.cFileName, TEXT("..")) != 0))) {
                TCHAR szTemp[MAX_PATH] = {0};
                StringCchCat(szTemp, MAX_PATH, dir.c_str());

                if (szTemp[_tcslen(szTemp) - 1] != TEXT('\\'))
                    StringCchCat(szTemp, MAX_PATH, TEXT("\\"));

                _tcscat_s(szTemp, MAX_PATH, filedata.cFileName);
                RecursiveKillProcess(szTemp, excludeSelf);
            }
        }
        else {
            TCHAR* p = _tcsrchr(filedata.cFileName, TEXT('.'));

            if (p) {
                if (_tcscmp(p, TEXT(".exe")) == 0) {
                    bool teminate = true;
                    if (excludeSelf) {
                        TCHAR szSelf[MAX_PATH] = {0};
                        GetModuleFileName(NULL, szSelf, MAX_PATH);
                        LPWSTR pSelf = PathFindFileName(szSelf);
                        if (_tcscmp(filedata.cFileName, pSelf) == 0)
                            teminate = false;
                    }
                    if (teminate) {
                        int tryTimes = 0;
                        while (KillProcess(filedata.cFileName) && ++tryTimes < 1000)
                            ;
                    }
                }
            }
        }

        while (FindNextFile(fhandle, &filedata) != 0) {
            if (filedata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (((_tcscmp(filedata.cFileName, TEXT(".")) != 0) &&
                     (_tcscmp(filedata.cFileName, TEXT("..")) != 0))) {
                    TCHAR szTemp[MAX_PATH] = {0};
                    StringCchCopy(szTemp, MAX_PATH, dir.c_str());

                    if (szTemp[_tcslen(szTemp) - 1] != TEXT('\\'))
                        StringCchCat(szTemp, MAX_PATH, TEXT("\\"));

                    StringCchCat(szTemp, MAX_PATH, filedata.cFileName);
                    RecursiveKillProcess(szTemp, excludeSelf);
                }
            }
            else {
                TCHAR* p = _tcsrchr(filedata.cFileName, TEXT('.'));

                if (p) {
                    if (_tcscmp(p, TEXT(".exe")) == 0) {
                        bool teminate = true;
                        if (excludeSelf) {
                            TCHAR szSelf[MAX_PATH] = {0};
                            GetModuleFileName(NULL, szSelf, MAX_PATH);
                            LPWSTR pSelf = PathFindFileName(szSelf);
                            if (_tcscmp(filedata.cFileName, pSelf) == 0)
                                teminate = false;
                        }
                        if (teminate) {
                            int tryTimes = 0;
                            while (KillProcess(filedata.cFileName) && ++tryTimes < 1000)
                                ;
                        }
                    }
                }
            }
        }
    }

    FindClose(fhandle);
}

void RecursiveKillProcess(const std::string& dir, bool excludeSelf) noexcept {
    RecursiveKillProcess(a2w(dir), excludeSelf);
}

std::wstring GetProcessPathW(unsigned long id) noexcept {
    std::wstring strPath;
    wchar_t szFilename[MAX_PATH] = {0};
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, id);
    if (hProcess == NULL)
        return strPath;

    HMODULE hModule = NULL;
    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
        if (GetModuleFileNameExW(hProcess, hModule, szFilename, MAX_PATH)) {
            strPath = szFilename;
        }
    }
    else {
        DWORD size = MAX_PATH;
        if (QueryFullProcessImageNameW(hProcess, 0, szFilename, &size)) {
            strPath = szFilename;
        }
    }
    CloseHandle(hProcess);

    return strPath;
}

std::string GetProcessPathA(unsigned long id) noexcept {
    std::string strPath;
    char szFilename[MAX_PATH] = {0};
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, id);
    if (hProcess == NULL)
        return strPath;

    HMODULE hModule = NULL;
    DWORD cbNeeded;
    if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbNeeded)) {
        if (GetModuleFileNameExA(hProcess, hModule, szFilename, MAX_PATH)) {
            strPath = szFilename;
        }
    }
    else {
        DWORD size = MAX_PATH;
        if (QueryFullProcessImageNameA(hProcess, 0, szFilename, &size)) {
            strPath = szFilename;
        }
    }
    CloseHandle(hProcess);

    return strPath;
}
#else
void KillProcessTree(pid_t id, bool force) noexcept {
    if (id <= 0)
        return;

    if (force)
        ::kill(-id, SIGTERM);
    else
        ::kill(-id, SIGINT);
}

bool KillProcess(pid_t id, bool force) noexcept {
    if (force)
        return ::kill(id, SIGTERM) == 0;
    return ::kill(id, SIGINT) == 0;
}

std::string GetProcessPathA(pid_t id) noexcept {
    std::string cmdLine;
    try {
        std::string cmdPath = std::string("/proc/") + std::to_string(id) + std::string("/cmdline");
        std::ifstream cmdFile(cmdPath.c_str());

        std::getline(cmdFile, cmdLine);
        if (!cmdLine.empty()) {
            // Keep first cmdline item which contains the program path
            size_t pos = cmdLine.find('\0');
            if (pos != string_type::npos)
                cmdLine = cmdLine.substr(0, pos);
        }
    } catch (std::exception& e) {
        ASHE_UNEXPECTED_EXCEPTION(e, L"GetProcessPathA");
    }
    return cmdLine;
}

std::wstring GetProcessPathW(pid_t id) noexcept {
    std::wstring cmdLine;
    try {
        std::wstring cmdPath = std::wstring(L"/proc/") + std::to_wstring(id) + std::wstring(L"/cmdline");
        std::wifstream cmdFile(cmdPath.c_str());

        std::getline(cmdFile, cmdLine);
        if (!cmdLine.empty()) {
            // Keep first cmdline item which contains the program path
            size_t pos = cmdLine.find(L'\0');
            if (pos != string_type::npos)
                cmdLine = cmdLine.substr(0, pos);
        }
    } catch (std::exception& e) {
        ASHE_UNEXPECTED_EXCEPTION(e, L"GetProcessPathW");
    }
    return cmdLine;
}
#endif

}  // namespace ashe