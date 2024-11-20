#include "ashe/os_util.h"
#include "ashe/process_util.h"
#ifdef ASHE_WIN
#include "ashe/win/windows_lite.h"
#endif  // ASHE_WIN

namespace ashe {
#ifdef ASHE_WIN
#ifndef NTSTATUS
#define NTSTATUS LONG
#endif

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#endif

typedef enum _SHUTDOWN_ACTION {
    ShutdownNoReboot,
    ShutdownReboot,
    ShutdownPowerOff
} SHUTDOWN_ACTION;

typedef NTSTATUS (NTAPI* __NtShutdownSystem)(__in SHUTDOWN_ACTION Action);
#else  // ASHE_WIN
#endif

#ifdef ASHE_WIN
bool OsFastReboot() {
    NTSTATUS status;
    __NtShutdownSystem shutdown_api = (__NtShutdownSystem)
        GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtShutdownSystem");
    if (shutdown_api == NULL) {
        return false;
    }

    if (!EnablePrivilege(SE_SHUTDOWN_NAME, true)) {
        return false;
    }

    status = shutdown_api(ShutdownReboot);
    if (!NT_SUCCESS(status)) {
        return false;
    }
    return true;
}

bool OsFastShutdown() {
    NTSTATUS status;
    __NtShutdownSystem shutdown_api = (__NtShutdownSystem)
        GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtShutdownSystem");
    if (shutdown_api == NULL) {
        return false;
    }

    if (!EnablePrivilege(SE_SHUTDOWN_NAME, true)) {
        return false;
    }

    status = shutdown_api(ShutdownPowerOff);
    if (!NT_SUCCESS(status)) {
        return false;
    }
    return true;
}
#else  // ASHE_WIN

#endif

}  // namespace ashe