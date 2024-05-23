#include "ashe/driver_info.h"

namespace ashe {
float DriverInfo::GetFreeMB(int nDrive) {
    std::wstring sRoot;
    unsigned long totalClusters, freeClusters, sectors, bytes;

    assert(nDrive > 0 && nDrive <= 26);

    if (IsDriveAvailable(nDrive)) {
        sRoot = GetRoot(nDrive);

        ::GetDiskFreeSpaceW(sRoot.c_str(), &sectors, &bytes, &freeClusters, &totalClusters);

        // do maths like this to avoid truncation
        // errors
        float fSpace = (float)sectors;
        fSpace *= bytes;
        fSpace *= freeClusters;
        fSpace /= 1048576;

        return fSpace;
    }

    return 0.0f;
}

std::wstring DriverInfo::GetVolume(int nDrive) {
    std::wstring sVolume, sRoot;

    assert(nDrive > 0 && nDrive <= 26);

    if (IsDriveAvailable(nDrive)) {
        sRoot = GetRoot(nDrive);

        wchar_t buf[20] = {0};
        GetVolumeInformationW(sRoot.c_str(), buf, 20, NULL, NULL, NULL, NULL, 0);

        std::wstring strVolume = buf;
        return sVolume;
    }

    return L"";
}

std::wstring DriverInfo::GetRoot(int nDrive) {
    std::wstring sRoot;

    sRoot = GetLetter(nDrive);
    sRoot += L":\\";

    return sRoot;
}

std::wstring DriverInfo::GetFullName(int nDrive) {
    std::wstring sFullName, sLetter, sVolume;

    assert(nDrive > 0 && nDrive <= 26);

    if (IsDriveAvailable(nDrive)) {
        sLetter = GetLetter(nDrive);
        sVolume = GetVolume(nDrive);

        sFullName = sVolume + L" (" + sLetter + L":)";

        return sFullName;
    }

    return L"";
}

float DriverInfo::GetTotalMB(int nDrive) {
    std::wstring sRoot;
    unsigned long totalClusters, freeClusters, sectors, bytes;

    assert(nDrive > 0 && nDrive <= 26);

    if (IsDriveAvailable(nDrive)) {
        sRoot = GetRoot(nDrive);

        ::GetDiskFreeSpaceW(sRoot.c_str(), &sectors, &bytes, &freeClusters, &totalClusters);

        // do maths like this to avoid truncation
        // errors
        float fSpace = (float)sectors;
        fSpace *= bytes;
        fSpace *= totalClusters;
        fSpace /= 1048576;

        return fSpace;
    }

    return 0.0f;
}

wchar_t DriverInfo::GetLetter(int nDrive) {
    assert(nDrive > 0 && nDrive <= 26);

    return (wchar_t)(nDrive + L'A' - 1);
}

int DriverInfo::GetType(int nDrive) {
    std::wstring sVolume;

    assert(nDrive > 0 && nDrive <= 26);

    // shortcut to avoid floppy access
    if (nDrive == 1 || nDrive == 2)
        return DRIVE_REMOVABLE;

    return ::GetDriveTypeW(GetRoot(nDrive).c_str());
}

int DriverInfo::GetPathType(const wchar_t* szPathName) {
    int nDrive = GetDrive(szPathName);

    if (nDrive >= 0)
        return GetType(nDrive);

    else if (IsRemotePath(szPathName) > 0)
        return DRIVE_REMOTE;

    else
        return DRIVE_UNKNOWN;
}

bool DriverInfo::IsDriveAvailable(int nDrive) {
    int nCurDrive;
    int nRes;

    // save cur drive and try to change to drive
    nCurDrive = _getdrive();
    nRes = _chdrive(nDrive);

    // if change successful change back before return
    if (nRes == 0)
        _chdrive(nCurDrive);

    return (nRes == 0) ? true : false;
}

bool DriverInfo::IsUNCPath(const wchar_t* szPathName) {
    return (wcsstr(szPathName, L"\\\\") == szPathName);
}

int DriverInfo::GetDrive(const wchar_t* szPathName) {
    int nDrive = 0;

    if (wcsstr(szPathName, L":") == szPathName + 1) {
        wchar_t cDrive = szPathName[0];
        cDrive = (wchar_t)toupper(cDrive);
        nDrive = cDrive - 64;
    }

    return nDrive ? nDrive : -1;
}

bool DriverInfo::IsMappedPath(const wchar_t* szPathName) {
    int nDrive;

    nDrive = GetDrive(szPathName);

    if (nDrive <= 0)
        return FALSE;

    return (GetType(nDrive) == DRIVE_REMOTE);
}

int DriverInfo::IsRemotePath(const wchar_t* szPathName, bool bAllowFileCheck) {
    if (bAllowFileCheck) {
        DWORD dwAttr = ::GetFileAttributesW(szPathName);

        if (dwAttr == 0xffffffff)
            return -1;
    }

    return (IsUNCPath(szPathName) || IsMappedPath(szPathName));
}

bool DriverInfo::IsFixedPath(const wchar_t* szPathName) {
    int nDrive = GetDrive(szPathName);

    if (nDrive == -1)  // unknown
        return FALSE;

    return (GetType(nDrive) == DRIVE_FIXED);
}

bool DriverInfo::IsRemovablePath(const wchar_t* szPathName) {
    int nDrive = GetDrive(szPathName);

    if (nDrive == -1)  // unknown
        return FALSE;

    return (GetType(nDrive) == DRIVE_REMOVABLE);
}

// -1 = no such path, else TRUE/FALSE
int DriverInfo::IsReadonlyPath(const wchar_t* szPathName) {
    DWORD dwAttr = ::GetFileAttributesW(szPathName);

    if (dwAttr == 0xffffffff)
        return -1;

    // else
    return (dwAttr & FILE_ATTRIBUTE_READONLY);
}

unsigned long DriverInfo::GetSerialNumber(int nDrive) {
    if (GetType(nDrive) != DRIVE_FIXED)
        return 0;

    DWORD dwHDSerialNum = 0;

    if (!::GetVolumeInformationW(GetRoot(nDrive).c_str(), NULL, 0, &dwHDSerialNum, NULL, NULL, NULL,
                                 0))
        return (DWORD)-1;

    return dwHDSerialNum;
}
}  // namespace ashe