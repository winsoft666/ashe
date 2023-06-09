#include "ashe/os_ver.hpp"

namespace ashe {
#ifdef ASHE_WIN
WinVerInfo OSVersion::GetWinVer() {
    WinVerInfo wvf;
    LONG(WINAPI * RtlGetVersion)
    (LPOSVERSIONINFOEX);
    *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");

    OSVERSIONINFOEX osversion;
    osversion.dwOSVersionInfoSize = sizeof(osversion);
    osversion.szCSDVersion[0] = TEXT('\0');

    if (RtlGetVersion != NULL) {
        // RtlGetVersion uses 0 (STATUS_SUCCESS) as return value when succeeding
        if (RtlGetVersion(&osversion) != 0)
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

bool OSVersion::IsWindowsVistaOrHigher() {
    const WinVerInfo wvi = GetWinVer();
    return (wvi.major >= 6);
}

bool OSVersion::IsWindows11() {
    const WinVerInfo wvi = GetWinVer();
    return (wvi.major == 10 && wvi.minor == 0 && wvi.build >= 22000);
}

bool OSVersion::IsWin64() {
    typedef BOOL(WINAPI * LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    static LPFN_ISWOW64PROCESS fnIsWow64Process = NULL;
    BOOL bIsWow64 = FALSE;

    if (NULL == fnIsWow64Process) {
        HMODULE h = GetModuleHandleW(L"kernel32");
        if (h)
            fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(h, "IsWow64Process");
    }

    if (NULL != fnIsWow64Process) {
        fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
    }

    return !!bIsWow64;
}
#endif

std::string OSVersion::GetOSVersion() {
#ifdef ASHE_WIN
    const WinVerInfo wvi = GetWinVer();
    char result[100] = {0};
    StringCchPrintfA(result, 100, "%d.%d.%d-%d", wvi.major, wvi.minor, wvi.build, wvi.productType);
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