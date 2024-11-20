#include "ashe/config.h"
#include "ashe/win/process_finder.h"
#include "ashe/string_encode.h"

namespace ashe {
namespace win {
ProcessFinder::ProcessFinder(DWORD dwFlags, DWORD dwProcessID) {
    m_hSnapShot = INVALID_HANDLE_VALUE;
    createSnapShot(dwFlags, dwProcessID);
}

ProcessFinder::~ProcessFinder() {
    if (m_hSnapShot != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hSnapShot);
        m_hSnapShot = INVALID_HANDLE_VALUE;
    }
}

bool ProcessFinder::createSnapShot(DWORD dwFlag, DWORD dwProcessID) {
    if (m_hSnapShot != INVALID_HANDLE_VALUE)
        CloseHandle(m_hSnapShot);

    if (dwFlag == 0)
        m_hSnapShot = INVALID_HANDLE_VALUE;
    else
        m_hSnapShot = CreateToolhelp32Snapshot(dwFlag, dwProcessID);

    return (m_hSnapShot != INVALID_HANDLE_VALUE);
}

bool ProcessFinder::processFirst(PPROCESSENTRY32 ppe) const {
    bool fOk = Process32First(m_hSnapShot, ppe);

    if (fOk && (ppe->th32ProcessID == 0))
        fOk = processNext(ppe);  // remove the "[System Process]" (PID = 0)

    return fOk;
}

bool ProcessFinder::processNext(PPROCESSENTRY32 ppe) const {
    bool fOk = Process32Next(m_hSnapShot, ppe);

    if (fOk && (ppe->th32ProcessID == 0))
        fOk = processNext(ppe);  // remove the "[System Process]" (PID = 0)

    return fOk;
}

// Don't forgot pe.dwSize = sizeof(PROCESSENTRY32);
bool ProcessFinder::processFind(DWORD dwProcessId, PPROCESSENTRY32 ppe) const {
    bool fFound = false;

    for (bool fOk = processFirst(ppe); fOk; fOk = processNext(ppe)) {
        fFound = (ppe->th32ProcessID == dwProcessId);

        if (fFound)
            break;
    }

    return fFound;
}

bool ProcessFinder::processFind(PCTSTR pszExeName, PPROCESSENTRY32 ppe, BOOL bExceptSelf) const {
    bool fFound = false;
    const DWORD dwCurrentPID = GetCurrentProcessId();

    for (bool fOk = processFirst(ppe); fOk; fOk = processNext(ppe)) {
        fFound = lstrcmpi(ppe->szExeFile, pszExeName) == 0;

        if (fFound) {
            if (bExceptSelf) {
                if (ppe->th32ProcessID == dwCurrentPID) {
                    fFound = false;
                    continue;
                }
            }
            break;
        }
    }

    return fFound;
}

bool ProcessFinder::moduleFirst(PMODULEENTRY32 pme) const {
    return (Module32First(m_hSnapShot, pme));
}

bool ProcessFinder::moduleNext(PMODULEENTRY32 pme) const {
    return (Module32Next(m_hSnapShot, pme));
}

bool ProcessFinder::moduleFind(PVOID pvBaseAddr, PMODULEENTRY32 pme) const {
    bool fFound = FALSE;

    for (bool fOk = moduleFirst(pme); fOk; fOk = moduleNext(pme)) {
        fFound = (pme->modBaseAddr == pvBaseAddr);

        if (fFound)
            break;
    }

    return fFound;
}

bool ProcessFinder::moduleFind(PTSTR pszModName, PMODULEENTRY32 pme) const {
    bool fFound = FALSE;

    for (bool fOk = moduleFirst(pme); fOk; fOk = moduleNext(pme)) {
        fFound =
            (lstrcmpi(pme->szModule, pszModName) == 0) || (lstrcmpi(pme->szExePath, pszModName) == 0);

        if (fFound)
            break;
    }

    return fFound;
}

bool ProcessFinder::IsExist(const std::wstring& processName) {
    ProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    const bool b = wpf.processFind(UnicodeToTCHAR(processName).c_str(), &pe32);
    return b;
}

bool ProcessFinder::IsExist(const std::string& processName) {
    ProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    const bool b = wpf.processFind(AnsiToTCHAR(processName).c_str(), &pe32);
    return b;
}

bool ProcessFinder::IsExist(const std::wstring& processName, DWORD* dwPID) {
    ProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    const bool b = wpf.processFind(UnicodeToTCHAR(processName).c_str(), &pe32);
    if (b) {
        if (dwPID)
            *dwPID = pe32.th32ProcessID;
    }
    return b;
}

bool ProcessFinder::IsExist(const std::string& processName, DWORD* dwPID) {
    ProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    const bool b = wpf.processFind(AnsiToTCHAR(processName).c_str(), &pe32);
    if (b) {
        if (dwPID)
            *dwPID = pe32.th32ProcessID;
    }
    return b;
}

bool ProcessFinder::IsExist(DWORD dwPID) {
    ProcessFinder wpf(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    bool b = wpf.processFind(dwPID, &pe32);
    return b;
}
}  // namespace win
}  // namespace ashe