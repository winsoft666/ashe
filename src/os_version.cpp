#include "ashe/os_version.h"

namespace ashe {
#ifdef ASHE_WIN
typedef LONG(WINAPI* RtlGetVersion)(LPOSVERSIONINFOEX);

WinVerInfo GetWinVer() noexcept {
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

bool IsWindowsVistaOrHigher() noexcept {
    const WinVerInfo wvi = GetWinVer();
    return (wvi.major >= 6);
}

bool IsWindows11() noexcept {
    const WinVerInfo wvi = GetWinVer();
    return (wvi.major >= 10 && wvi.build >= 22000);
}

bool IsWin64() noexcept {
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

bool IsWow64(HANDLE process, bool& result) noexcept {
    BOOL bIsWow64 = FALSE;

    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process =
        (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

    if (NULL == fnIsWow64Process) {
        return false;
    }

    if (!fnIsWow64Process(process, &bIsWow64)) {
        return false;
    }

    result = !!bIsWow64;
    return true;
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