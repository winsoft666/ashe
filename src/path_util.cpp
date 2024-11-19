#include "ashe/config.h"
#include "ashe/path_util.h"

#ifdef ASHE_WIN
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <shlobj_core.h>
#include <shellapi.h>
#include <Shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")
#else
#include <unistd.h>
#endif
#include "ashe/os_version.h"
#include "ashe/string_helper.h"
#include "ashe/check_failure.h"

namespace ashe {
#ifdef ASHE_WIN
std::wstring GetWindowsFolder() {
    wchar_t szBuf[MAX_PATH] = {0};
    const DWORD result = ::GetWindowsDirectoryW(szBuf, MAX_PATH);
    if (result == 0)
        return std::wstring();

    std::wstring tempPath = szBuf;
    if (!tempPath.empty())
        if (tempPath.back() != L'\\')
            tempPath.push_back(L'\\');
    return tempPath;
}

std::wstring GetSystemFolder() {
    wchar_t szBuf[MAX_PATH] = {0};
    const DWORD result = ::GetSystemDirectoryW(szBuf, MAX_PATH);
    if (result == 0)
        return std::wstring();

    std::wstring tempPath = szBuf;
    if (!tempPath.empty())
        if (tempPath.back() != L'\\')
            tempPath.push_back(L'\\');
    return tempPath;
}

std::wstring GetTempFolder() {
    wchar_t szBuf[MAX_PATH] = {0};
    const DWORD result = ::GetTempPathW(MAX_PATH, szBuf);
    if (result == 0)
        return std::wstring();

    std::wstring tempPath = szBuf;
    if (!tempPath.empty())
        if (tempPath.back() != L'\\')
            tempPath.push_back(L'\\');
    return tempPath;
}

std::wstring GetCurrentUserDesktopFolder() {
    wchar_t szDesktopDir[MAX_PATH] = {0};
    SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_DESKTOPDIRECTORY, 0);
    PathAddBackslashW(szDesktopDir);

    return szDesktopDir;
}

std::wstring GetAllUserDesktopFolder() {
    wchar_t szDesktopDir[MAX_PATH] = {0};
    SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_COMMON_DESKTOPDIRECTORY, 0);
    PathAddBackslashW(szDesktopDir);

    return szDesktopDir;
}

std::wstring GetCurrentUserProgramsFolder() {
    wchar_t szDesktopDir[MAX_PATH] = {0};
    SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_PROGRAMS, 0);
    PathAddBackslashW(szDesktopDir);

    return szDesktopDir;
}

std::wstring GetAllUserProgramsFolder() {
    wchar_t szDesktopDir[MAX_PATH] = {0};
    SHGetSpecialFolderPathW(NULL, szDesktopDir, CSIDL_COMMON_PROGRAMS, 0);
    PathAddBackslashW(szDesktopDir);

    return szDesktopDir;
}

std::wstring GetProgramFilesx86Folder() {
    wchar_t szDir[MAX_PATH] = {0};
    SHGetSpecialFolderPathW(NULL, szDir, CSIDL_PROGRAM_FILESX86, 0);
    PathAddBackslashW(szDir);

    return szDir;
}

std::wstring GetProgramFilesFolder() {
    wchar_t szDir[MAX_PATH] = {0};
    SHGetSpecialFolderPathW(NULL, szDir, CSIDL_PROGRAM_FILES, 0);
    PathAddBackslashW(szDir);

    return szDir;
}

std::wstring GetLocalAppDataFolder() {
#if (NTDDI_VERSION < NTDDI_VISTA)
#ifndef KF_FLAG_CREATE
#define KF_FLAG_CREATE 0x00008000
#endif
#endif
    std::wstring tempPath;
    if (IsWindowsVistaOrHigher()) {
        typedef HRESULT(WINAPI * __SHGetKnownFolderPath)(REFKNOWNFOLDERID, DWORD, HANDLE, PWSTR*);
        HMODULE hDll = ::LoadLibraryW(L"shell32.dll");
        if (hDll != NULL) {
            __SHGetKnownFolderPath _SHGetKnownFolderPath =
                reinterpret_cast<__SHGetKnownFolderPath>(::GetProcAddress(hDll, "SHGetKnownFolderPath"));
            if (_SHGetKnownFolderPath != NULL) {
                PWSTR result = NULL;
                if (S_OK == _SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &result)) {
                    tempPath = result;
                    ::CoTaskMemFree(result);
                }
            }
            ::FreeLibrary(hDll);
        }
    }
    else {
        // On Windows XP, CSIDL_LOCAL_APPDATA represents "{user}\Local Settings\Application Data"
        // while CSIDL_APPDATA represents "{user}\Application Data"
        wchar_t buffer[MAX_PATH] = {0};
        if (S_OK == ::SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, buffer))
            tempPath = buffer;
    }

    if (!tempPath.empty())
        if (tempPath.back() != L'\\')
            tempPath.push_back(L'\\');

    return tempPath;
}

bool OpenWinExplorerAndLocate(const std::wstring& path) {
    bool result = false;
    PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(path.c_str());
    if (pidl) {
        result = (S_OK == SHOpenFolderAndSelectItems(pidl, 0, NULL, 0));
        ILFree(pidl);
    }
    return result;
}

std::wstring GetWinExplorerDisplayName(const std::wstring& path) {
    SHFILEINFOW sfi;
    ZeroMemory(&sfi, sizeof(SHFILEINFOW));
    DWORD_PTR dwRet = ::SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFOW), SHGFI_DISPLAYNAME);
    if (dwRet != 0) {
        return std::wstring(sfi.szDisplayName);
    }
    return std::wstring();
}

std::string ReplaceKnownEnvToWow6432(const std::string& src) {
    if (!IsContains(src, "%"))
        return src;

    BOOL isWow64 = FALSE;
#ifdef ASHE_WIN32
    HANDLE hProcess = GetCurrentProcess();
    IsWow64Process(hProcess, &isWow64);
#endif

    if (!isWow64)
        return src;

    std::string sl = src;
    sl = StrReplace(sl, "%ProgramFiles%", "%ProgramW6432%", 0, true);
    sl = StrReplace(sl, "%CommonProgramFiles%", "%CommonProgramW6432%", 0, true);

    return sl;
}

std::wstring ReplaceKnownEnvToWow6432(const std::wstring& src) {
    if (!IsContains(src, L"%"))
        return src;

    BOOL isWow64 = FALSE;
#ifdef ASHE_WIN32
    HANDLE hProcess = GetCurrentProcess();
    IsWow64Process(hProcess, &isWow64);
#endif

    if (!isWow64)
        return src;

    std::wstring sl = src;
    sl = StrReplace(sl, L"%ProgramFiles%", L"%ProgramW6432%", 0, true);
    sl = StrReplace(sl, L"%CommonProgramFiles%", L"%CommonProgramW6432%", 0, true);

    return sl;
}

std::string ExpandEnvString(const std::string& src, bool disableWow64FsRedirection) {
    if (src.empty()) {
        return "";
    }

    std::string srcCopy;
    PVOID OldValue = NULL;
    if (disableWow64FsRedirection) {
        Wow64DisableWow64FsRedirection(&OldValue);
        srcCopy = ReplaceKnownEnvToWow6432(src);
    }
    else {
        srcCopy = src;
    }

    DWORD dwCount = ExpandEnvironmentStringsA(srcCopy.c_str(), NULL, 0);
    if (dwCount == 0) {
        if (disableWow64FsRedirection)
            Wow64RevertWow64FsRedirection(OldValue);
        return "";
    }

    std::vector<char> buf(dwCount + 1);
    ExpandEnvironmentStringsA(srcCopy.c_str(), &buf[0], dwCount);

    if (disableWow64FsRedirection)
        Wow64RevertWow64FsRedirection(OldValue);

    return std::string(&buf[0]);
}

std::wstring ExpandEnvString(const std::wstring& src, bool disableWow64FsRedirection) {
    if (src.empty()) {
        return L"";
    }

    std::wstring srcCopy;
    PVOID OldValue = NULL;
    if (disableWow64FsRedirection) {
        Wow64DisableWow64FsRedirection(&OldValue);
        srcCopy = ReplaceKnownEnvToWow6432(src);
    }
    else {
        srcCopy = src;
    }

    DWORD dwCount = ExpandEnvironmentStringsW(srcCopy.c_str(), NULL, 0);
    if (dwCount == 0) {
        if (disableWow64FsRedirection)
            Wow64RevertWow64FsRedirection(OldValue);
        return L"";
    }

    std::vector<wchar_t> buf(dwCount + 1);
    ExpandEnvironmentStringsW(srcCopy.c_str(), &buf[0], dwCount);

    if (disableWow64FsRedirection)
        Wow64RevertWow64FsRedirection(OldValue);

    return std::wstring(&buf[0]);
}

#endif

bool PathIsSurroundQuote(const wchar_t* szPath) {
    if (!szPath)
        return false;

    size_t oldLen = wcslen(szPath);
    if (wcslen(szPath) < 2)
        return false;

    if ((szPath[0] == L'"' && szPath[oldLen - 1] == L'"') || (szPath[0] == L'\'' && szPath[oldLen - 1] == L'\'')) {
        return true;
    }

    return false;
}

bool PathIsSurroundQuote(const std::wstring& path) {
    size_t oldLen = path.length();
    if (oldLen < 2)
        return false;

    if ((path[0] == L'"' && path[oldLen - 1] == L'"') || (path[0] == L'\'' && path[oldLen - 1] == L'\'')) {
        return true;
    }
    return false;
}

void PathRemoveQuote(wchar_t* szPath) {
    if (PathIsSurroundQuote(szPath)) {
        szPath[wcslen(szPath) - 1] = 0;
        wcscpy(szPath, szPath + 1);
    }
}

std::wstring PathRemoveQuote(const std::wstring& path) {
    if (PathIsSurroundQuote(path)) {
        return path.substr(1, path.length() - 2);
    }

    return path;
}

bool PathAddQuote(wchar_t* szPath, size_t buffSize, bool singleQuote) {
    if (PathIsSurroundQuote(szPath))
        return true;

    size_t oldLen = wcslen(szPath);
    if (oldLen + 2 >= buffSize)
        return false;

    wchar_t quote = singleQuote ? L'\'' : L'"';

    wchar_t* buff = new (std::nothrow) wchar_t[oldLen + 1];
    if (!buff)
        return false;

    wcscpy(buff, szPath);
    buff[oldLen] = 0;

    szPath[0] = quote;
    wcscpy(szPath + 1, buff);
    szPath[oldLen + 1] = quote;
    szPath[oldLen + 2] = 0;

    delete[] buff;

    return true;
}

std::wstring PathAddQuote(const std::wstring& path, bool singleQuote) {
    if (PathIsSurroundQuote(path))
        return path;

    wchar_t quote = singleQuote ? L'\'' : L'"';
    return std::wstring(1, quote) + path + std::wstring(1, quote);
}

std::wstring PathGetDirectory(const std::wstring& path, int upLevel) {
    try {
        std::wstring dir = path;
        for (int i = 0; i < upLevel; i++) {
            if (dir.empty())
                break;

            size_t offset = std::wstring::npos;
            wchar_t end = dir[dir.size() - 1];
            if (end == L'\\' || end == L'/') {
                offset = dir.size() - 2;
            }

            size_t pos = dir.find_last_of(L"\\/", offset);
            if (pos != std::wstring::npos)
                pos += 1;
            dir = dir.substr(0, pos);
        }
        return dir;
    } catch (std::exception& e) {
        ASHE_UNEXPECTED_EXCEPTION(e, nullptr);
        return L"";
    }
}

std::wstring PathGetFileName(const std::wstring& path) {
    try {
        return path.substr(path.find_last_of(L"\\/") + 1);
    } catch (std::exception& e) {
        ASHE_UNEXPECTED_EXCEPTION(e, nullptr);
        return L"";
    }
}

std::wstring PathJoin(const std::wstring& path1, const std::wstring& path2) {
    if (path1.empty())
        return path2;

    if (path2.empty())
        return path1;

    std::wstring res = path1;
    wchar_t end = res[res.size() - 1];
    if (end != L'\\' && end != L'/') {
#ifdef ASHE_WIN
        res += L"\\";
#else
        res += L"/";
#endif
    }

    wchar_t start = path2[0];
    if (start == L'\\' || start == L'/') {
        res += path2.substr(1);
    }
    else {
        res += path2;
    }

    return res;
}

bool PathIsExist(const std::wstring& path) {
#ifdef ASHE_WIN
    WIN32_FILE_ATTRIBUTE_DATA attrs = {0};
    return GetFileAttributesExW(path.c_str(), GetFileExInfoStandard, &attrs) != 0;
#else
    return (access(w2u(path).c_str(), F_OK) == 0);
#endif
}

bool PathIsFile(const std::wstring& path) {
#ifdef ASHE_WIN
    DWORD attrs = GetFileAttributesW(path.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        if (!(attrs & FILE_ATTRIBUTE_DIRECTORY))
            return true;
    }
    return false;
#else
    // TODO
#endif
}

bool PathIsDirectory(const std::wstring& path) {
#ifdef ASHE_WIN
    DWORD attrs = GetFileAttributesW(path.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        if (attrs & FILE_ATTRIBUTE_DIRECTORY)
            return true;
    }
    return false;
#else
    // TODO
#endif
}

bool MakePath(const std::wstring& path) {
#ifdef ASHE_WIN
    wchar_t* pathDup = _wcsdup(path.c_str());

    bool done = false;
    wchar_t* slash = pathDup;

    while (!done) {
        slash += wcsspn(slash, L"\\");
        slash += wcscspn(slash, L"\\");

        done = (*slash == L'\0');
        *slash = L'\0';

        auto attribute = GetFileAttributes(pathDup);
        if (attribute == INVALID_FILE_ATTRIBUTES) {
            if (!CreateDirectory(pathDup, nullptr)) {
                free(pathDup);
                return false;
            }
        }
        else if (!(attribute & FILE_ATTRIBUTE_DIRECTORY)) {
            free(pathDup);
            return false;
        }

        *slash = L'\\';
    }
    free(pathDup);
    return true;
#elif !defined(ASHE_MACOS)
    std::string u8Path = w2u(path);
    char* pathDup = strdup(u8Path.c_str());

    struct stat sb = {};
    bool done = false;
    char* slash = pathDup;

    while (!done) {
        slash += strspn(slash, "/");
        slash += strcspn(slash, "/");

        done = (*slash == '\0');
        *slash = '\0';

        if (stat(pathDup, &sb) != 0) {
            if (errno != ENOENT || mkdir(pathDup, 0777) != 0) {
                // there's report that some Android devices might not have access permission on parent dir
                if (done) {
                    free(pathDup);
                    return false;
                }
                goto LContinue;
            }
        }
        else if (!S_ISDIR(sb.st_mode)) {
            free(pathDup);
            return false;
        }
    LContinue:
        *slash = '/';
    }
    free(pathDup);

    return true;
#endif
}

}  // namespace ashe