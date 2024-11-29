#include "ashe/os_version.h"

namespace ashe {
#ifdef ASHE_WIN
typedef LONG(WINAPI* RtlGetVersion)(LPOSVERSIONINFOEX);

WinVerInfo GetWinVer() {
    WinVerInfo wvf;
    RtlGetVersion fn = nullptr;
    HMODULE hDll = GetModuleHandleA("ntdll.dll");
    if (hDll) {
        fn = (RtlGetVersion)GetProcAddress(hDll, "RtlGetVersion");
    }
    OSVERSIONINFOEX osversion;
    osversion.dwOSVersionInfoSize = sizeof(osversion);
    osversion.szCSDVersion[0] = TEXT('\0');

    if (fn != NULL) {
        // RtlGetVersion uses 0 (STATUS_SUCCESS) as return value when succeeding
        if (fn(&osversion) != 0)
            return wvf;
    }
    else {
        // GetVersionEx was deprecated in Windows 10, only use it as fallback
        OSVERSIONINFOEX osversion;
        osversion.dwOSVersionInfoSize = sizeof(osversion);
        osversion.szCSDVersion[0] = TEXT('\0');
        if (!GetVersionEx((LPOSVERSIONINFO)&osversion))
            return wvf;
    }

    wvf.major = (int)osversion.dwMajorVersion;
    wvf.minor = (int)osversion.dwMinorVersion;
    wvf.build = (int)osversion.dwBuildNumber;

    DWORD dwProductType = 0;
    if (GetProductInfo(osversion.dwMajorVersion, osversion.dwMinorVersion, 0, 0, &dwProductType)) {
        wvf.productType = (int)dwProductType;
    }
    return wvf;
}

bool IsWindowsVistaOrHigher() {
    const WinVerInfo wvi = GetWinVer();
    return (wvi.major >= 6);
}

bool IsWindows7() {
    const WinVerInfo wvi = GetWinVer();
    return (wvi.major == 6 && wvi.minor == 1);
}

bool IsWindows8OrHigher() {
    bool result = false;
    const WinVerInfo wvi = GetWinVer();

    if (wvi.major > 6) {
        result = true;
    }
    else if (wvi.major == 6) {
        result = (wvi.minor >= 2);
    }

    return result;
}

bool IsWindows10OrHigher() {
    const WinVerInfo wvi = GetWinVer();
    return (wvi.major >= 10);
}

bool IsWindows11() {
    bool result = false;
    const WinVerInfo wvi = GetWinVer();

    if (wvi.major > 10) {
         result = true;
    }
    else if (wvi.major == 10) {
        result = (wvi.build >= 22000);
    }

    return result;
}

bool IsWin64() {
#if 0
#ifdef ASHE_WIN64  // 64bit process
    return true;
#else              // 32bit process
    bool result = false;
    IsWow64(GetCurrentProcess(), result);
    return result;
#endif
#else
    bool result = false;
    SYSTEM_INFO si;
    RtlZeroMemory(&si, sizeof(SYSTEM_INFO));
    GetNativeSystemInfo(&si);
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        result = true;
    return result;
#endif
}

#endif

std::string GetOSVersion() {
#ifdef ASHE_WIN
    const WinVerInfo wvi = GetWinVer();
    char result[200] = {0};
    StringCchPrintfA(result, 200, "%d.%d.%d-%d", wvi.major, wvi.minor, wvi.build, wvi.productType);
    return result;
#elif defined(ASHE_MACOS)
    char result[1024] = {0};
    size_t size = sizeof(result);
    if (sysctlbyname("kern.osrelease", result, &size, nullptr, 0) == 0)
        return result;
    return "<apple>";

#elif defined(ASHE_LINUX)
    static std::regex pattern("DISTRIB_DESCRIPTION=\"(.*)\"");

    std::string line;
    std::ifstream stream("/etc/lsb-release");
    while (getline(stream, line)) {
        std::smatch matches;
        if (std::regex_match(line, matches, pattern))
            return matches[1];
    }

    return "<linux>";
#endif
}
}  // namespace ashe