#include "ashe/config.hpp"
#include "ashe/win_icon.hpp"
#ifdef ASHE_WIN
#include <strsafe.h>
#include <Shlwapi.h>

namespace ashe {
namespace {
typedef UINT(WINAPI* fPrivateExtractIconsW)(
    IN LPCWSTR szFileName,
    IN int nIconIndex,
    IN int cxIcon,
    IN int cyIcon,
    OUT HICON* phicon,
    OUT UINT* piconid,
    IN UINT nIcons,
    IN UINT flags);

struct EMUN_ICONS {
    int index = 0;
    WCHAR* name = NULL;
};

BOOL CALLBACK __EnumIconGroupCallback__(HMODULE hModule, LPCWSTR lpszType, LPWSTR lpszName, LONG_PTR lParam) {
    std::vector<WCHAR*>* pResList = (std::vector<WCHAR*>*)lParam;
    if (pResList) {
        if (lpszName) {
            if (IS_INTRESOURCE(lpszName)) {
                pResList->push_back(lpszName);
            }
            else {
                int len = wcslen(lpszName);
                WCHAR* pBuf = new WCHAR[len + 1];
                StringCchCopyW(pBuf, len + 1, lpszName);
                pResList->push_back(pBuf);
            }
        }
    }
    return TRUE;
}

typedef struct
{
    WORD idReserved;  // must be 0
    WORD idType;      // 1 = ICON, 2 = CURSOR
    WORD idCount;     // number of images (and ICONDIRs)

    // ICONDIR [1...n]
    // ICONIMAGE [1...n]

} ICONHEADER;

//
// An array of ICONDIRs immediately follow the ICONHEADER
//
typedef struct
{
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;    // for cursors, this field = wXHotSpot
    WORD wBitCount;  // for cursors, this field = wYHotSpot
    DWORD dwBytesInRes;
    DWORD dwImageOffset;  // file-offset to the start of ICONIMAGE

} ICONDIR;

//
// After the ICONDIRs follow the ICONIMAGE structures -
// consisting of a BITMAPINFOHEADER, (optional) RGBQUAD array, then
// the color and mask bitmap bits (all packed together
//
typedef struct
{
    BITMAPINFOHEADER biHeader;  // header for color bitmap (no mask header)
    //RGBQUAD rgbColors[1...n];
    //BYTE bXOR[1]; // DIB bits for color bitmap
    //BYTE bAND[1]; // DIB bits for mask bitmap

} ICONIMAGE;

//
// Write the ICO header to disk
//
static UINT WriteIconHeader(HANDLE hFile, int nImages) {
    ICONHEADER iconheader;
    DWORD nWritten;

    // Setup the icon header
    iconheader.idReserved = 0;     // Must be 0
    iconheader.idType = 1;         // Type 1 = ICON (type 2 = CURSOR)
    iconheader.idCount = nImages;  // number of ICONDIRs

    // Write the header to disk
    WriteFile(hFile, &iconheader, sizeof(iconheader), &nWritten, 0);

    // following ICONHEADER is a series of ICONDIR structures (idCount of them, in fact)
    return nWritten;
}

//
// Return the number of BYTES the bitmap will take ON DISK
//
static UINT NumBitmapBytes(BITMAP* pBitmap) {
    int nWidthBytes = pBitmap->bmWidthBytes;

    // bitmap scanlines MUST be a multiple of 4 bytes when stored
    // inside a bitmap resource, so round up if necessary
    if (nWidthBytes & 3)
        nWidthBytes = (nWidthBytes + 4) & ~3;

    return nWidthBytes * pBitmap->bmHeight;
}

//
// Return number of bytes written
//
static UINT WriteIconImageHeader(HANDLE hFile, BITMAP* pbmpColor, BITMAP* pbmpMask) {
    BITMAPINFOHEADER biHeader;
    DWORD nWritten;
    UINT nImageBytes;

    // calculate how much space the COLOR and MASK bitmaps take
    nImageBytes = NumBitmapBytes(pbmpColor) + NumBitmapBytes(pbmpMask);

    // write the ICONIMAGE to disk (first the BITMAPINFOHEADER)
    ZeroMemory(&biHeader, sizeof(biHeader));

    // Fill in only those fields that are necessary
    biHeader.biSize = sizeof(biHeader);
    biHeader.biWidth = pbmpColor->bmWidth;
    biHeader.biHeight = pbmpColor->bmHeight * 2;  // height of color+mono
    biHeader.biPlanes = pbmpColor->bmPlanes;
    biHeader.biBitCount = pbmpColor->bmBitsPixel;
    biHeader.biSizeImage = nImageBytes;

    // write the BITMAPINFOHEADER
    WriteFile(hFile, &biHeader, sizeof(biHeader), &nWritten, 0);

    // write the RGBQUAD color table (for 16 and 256 colour icons)
    if (pbmpColor->bmBitsPixel == 2 || pbmpColor->bmBitsPixel == 8) {
    }

    return nWritten;
}

//
// Wrapper around GetIconInfo and GetObject(BITMAP)
//
static BOOL GetIconBitmapInfo(HICON hIcon, ICONINFO* pIconInfo, BITMAP* pbmpColor, BITMAP* pbmpMask) {
    if (!GetIconInfo(hIcon, pIconInfo))
        return FALSE;

    if (!GetObject(pIconInfo->hbmColor, sizeof(BITMAP), pbmpColor))
        return FALSE;

    if (!GetObject(pIconInfo->hbmMask, sizeof(BITMAP), pbmpMask))
        return FALSE;

    return TRUE;
}

//
// Write one icon directory entry - specify the index of the image
//
static UINT WriteIconDirectoryEntry(HANDLE hFile, int nIdx, HICON hIcon, UINT nImageOffset) {
    ICONINFO iconInfo;
    ICONDIR iconDir;

    BITMAP bmpColor;
    BITMAP bmpMask;

    DWORD nWritten;
    UINT nColorCount;
    UINT nImageBytes;

    GetIconBitmapInfo(hIcon, &iconInfo, &bmpColor, &bmpMask);

    nImageBytes = NumBitmapBytes(&bmpColor) + NumBitmapBytes(&bmpMask);

    if (bmpColor.bmBitsPixel >= 8)
        nColorCount = 0;
    else
        nColorCount = 1 << (bmpColor.bmBitsPixel * bmpColor.bmPlanes);

    // Create the ICONDIR structure
    iconDir.bWidth = (BYTE)bmpColor.bmWidth;
    iconDir.bHeight = (BYTE)bmpColor.bmHeight;
    iconDir.bColorCount = nColorCount;
    iconDir.bReserved = 0;
    iconDir.wPlanes = bmpColor.bmPlanes;
    iconDir.wBitCount = bmpColor.bmBitsPixel;
    iconDir.dwBytesInRes = sizeof(BITMAPINFOHEADER) + nImageBytes;
    iconDir.dwImageOffset = nImageOffset;

    // Write to disk
    WriteFile(hFile, &iconDir, sizeof(iconDir), &nWritten, 0);

    // Free resources
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);

    return nWritten;
}

static UINT WriteIconData(HANDLE hFile, HBITMAP hBitmap) {
    BITMAP bmp;
    int i;
    BYTE* pIconData;

    UINT nBitmapBytes;
    DWORD nWritten;

    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    nBitmapBytes = NumBitmapBytes(&bmp);

    pIconData = (BYTE*)malloc(nBitmapBytes);

    GetBitmapBits(hBitmap, nBitmapBytes, pIconData);

    // bitmaps are stored inverted (vertically) when on disk..
    // so write out each line in turn, starting at the bottom + working
    // towards the top of the bitmap. Also, the bitmaps are stored in packed
    // in memory - scanlines are NOT 32bit aligned, just 1-after-the-other
    for (i = bmp.bmHeight - 1; i >= 0; i--) {
        // Write the bitmap scanline
        WriteFile(
            hFile,
            pIconData + (i * bmp.bmWidthBytes),  // calculate offset to the line
            bmp.bmWidthBytes,                    // 1 line of BYTES
            &nWritten,
            0);

        // extend to a 32bit boundary (in the file) if necessary
        if (bmp.bmWidthBytes & 3) {
            DWORD padding = 0;
            WriteFile(hFile, &padding, 4 - bmp.bmWidthBytes, &nWritten, 0);
        }
    }

    free(pIconData);

    return nBitmapBytes;
}
}  // namespace

HICON WinIcon::LoadFromProcessRes(HINSTANCE hInst, LPCWSTR resPath, int cxDesired, int cyDesired) noexcept {
    HANDLE h = LoadImageW(hInst, resPath, IMAGE_ICON, cxDesired, cyDesired, LR_DEFAULTCOLOR);
    if (!h) {
        return NULL;
    }

    HICON hIcon = (HICON)h;
    if (!hIcon) {
        return NULL;
    }

    HICON hCopy = CopyIcon(hIcon);
    DestroyIcon(hIcon);
    return hCopy;
}

HICON WinIcon::LoadFromProcessRes(const std::wstring& filePath, LPCWSTR resPath, int cxDesired /*= 0*/, int cyDesired /*= 0*/) noexcept {
    if (filePath.empty())
        return NULL;

    HMODULE hMod = LoadLibraryExW(filePath.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!hMod)
        return NULL;

    HICON hIcon = LoadFromProcessRes(hMod, resPath, cxDesired, cyDesired);

    FreeLibrary(hMod);

    return hIcon;
}

bool WinIcon::ParseShell32IconInfo(const std::wstring& shell32Path, int shellIconIndex, std::wstring& iconPath, int& iconIndex) noexcept {
    if (shell32Path.empty()) {
        return false;
    }

    HKEY key = NULL;
    if (RegOpenKeyW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Icons", &key) != ERROR_SUCCESS) {
        return false;
    }

    WCHAR szShellIconIndex[10] = {0};
    StringCchPrintfW(szShellIconIndex, 10, L"%d", shellIconIndex);

    WCHAR substFile[MAX_PATH] = {0};
    DWORD dwType = REG_SZ;
    DWORD size = sizeof(substFile);
    if (RegQueryValueExW(key, szShellIconIndex, NULL, &dwType, (LPBYTE)substFile, &size) == ERROR_SUCCESS) {
        RegCloseKey(key);
        return false;
    }

    WCHAR icFile[MAX_PATH] = {0};
    DWORD icIDX = PathParseIconLocationW(substFile);
    ExpandEnvironmentStringsW(substFile, icFile, MAX_PATH);

    iconPath = icFile;
    iconIndex = icIDX;

    RegCloseKey(key);

    return true;
}

bool WinIcon::EnumIconGroups(const std::wstring& filePath, std::vector<IconGroup>& iconGroups) {
    if (filePath.empty()) {
        return false;
    }

    HMODULE hMod = LoadLibraryExW(filePath.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE | LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!hMod) {
        return false;
    }

    std::vector<WCHAR*> resourceList;
    if (!EnumResourceNamesW(hMod, RT_GROUP_ICON, __EnumIconGroupCallback__, (LONG_PTR)&resourceList)) {
        return false;
    }

    iconGroups.clear();
    for (size_t i = 0; i < resourceList.size(); i++) {
        IconGroup iconGroup;

        WCHAR* resName = resourceList[i];
        if (IS_INTRESOURCE(resName)) {
            iconGroup.name = std::to_wstring((long long)resName);
        }
        else {
            iconGroup.name = resName;
        }

        HRSRC hResource = FindResourceW(hMod, resName, RT_GROUP_ICON);
        if (!hResource) {
            if (!IS_INTRESOURCE(resName)) {
                delete[] resName;
            }
            continue;
        }

        HGLOBAL hMem = LoadResource(hMod, hResource);
        if (!hMem) {
            if (!IS_INTRESOURCE(resName)) {
                delete[] resName;
            }
            continue;
        }

        PBYTE lpResource = (PBYTE)LockResource(hMem);
        if (!lpResource) {
            if (!IS_INTRESOURCE(resName)) {
                delete[] resName;
            }
            continue;
        }

        const std::vector<int> standardIconSize = {16, 32, 48, 64, 128, 256};
        for (const int iconSize : standardIconSize) {
            const int iconId = LookupIconIdFromDirectoryEx(lpResource, TRUE, iconSize, iconSize, LR_DEFAULTCOLOR);
            if (iconId == 0)
                continue;

            HRSRC hRes2 = FindResourceW(hMod, MAKEINTRESOURCE(iconId), MAKEINTRESOURCE(RT_ICON));
            if (!hRes2)
                continue;

            HGLOBAL hMemory2 = LoadResource(hMod, hRes2);
            if (!hMemory2)
                continue;

            PBYTE lpRes2 = (PBYTE)LockResource(hMemory2);
            if (!lpRes2)
                continue;

            DWORD dwResSize = SizeofResource(hMod, hRes2);
            HICON hIcon = CreateIconFromResourceEx(lpRes2, dwResSize, TRUE, 0x00030000, iconSize, iconSize, LR_DEFAULTCOLOR);
            if (!hIcon)
                continue;

            std::shared_ptr<IconInfo> iconInfo = std::make_shared<IconInfo>();
            iconInfo->hIcon = CopyIcon(hIcon);
            iconInfo->cx = iconSize;
            iconInfo->cy = iconSize;

            DestroyIcon(hIcon);

            iconGroup.icons.push_back(iconInfo);
        }

        iconGroups.push_back(iconGroup);

        if (!IS_INTRESOURCE(resName)) {
            delete[] resName;
        }
    }

    FreeLibrary(hMod);

    return true;
}

HICON WinIcon::GetExeIcon(const std::wstring& filePath, int desiredSize, int &actualSize) noexcept {
    std::vector<IconGroup> iconGroups;
    if (!EnumIconGroups(filePath, iconGroups)) {
        return NULL;
    }

    if (iconGroups.size() == 0) {
        return NULL;
    }

    int closestValue = 0;
    size_t idx = -1;
    for (size_t i = 0; i < iconGroups[0].icons.size(); i++) {
        if (iconGroups[0].icons[i]) {
            int v = abs(desiredSize - iconGroups[0].icons[i]->cx);
            if (idx == -1) {
                idx = i;
                closestValue = v;
            }
            else if (closestValue > v) {
                idx = i;
                closestValue = v;
            }
        }
    }

    if (idx == -1) {
        return NULL;
    }

    actualSize = iconGroups[0].icons[idx]->cx;
    HICON hCopy = CopyIcon(iconGroups[0].icons[idx]->hIcon);
    return hCopy;
}

bool WinIcon::SaveToFile(const std::vector<HICON>& hIcons, const std::wstring& filePath) noexcept {
    HANDLE hFile = INVALID_HANDLE_VALUE;
    int* pImageOffset = NULL;

    if (hIcons.size() == 0 || filePath.empty()) {
        return false;
    }

    for (size_t i = 0; i < hIcons.size(); i++) {
        if (!hIcons[i]) {
            return false;
        }
    }

    hFile = CreateFile(filePath.c_str(), GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);

    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    if (WriteIconHeader(hFile, hIcons.size()) == 0) {
        CloseHandle(hFile);
        return false;
    }

    SetFilePointer(hFile, sizeof(ICONDIR) * hIcons.size(), 0, FILE_CURRENT);

    pImageOffset = (int*)malloc(hIcons.size() * sizeof(int));
    if (!pImageOffset) {
        CloseHandle(hFile);
        return false;
    }

    for (size_t i = 0; i < hIcons.size(); i++) {
        ICONINFO iconInfo;
        BITMAP bmpColor, bmpMask;

        GetIconBitmapInfo(hIcons[i], &iconInfo, &bmpColor, &bmpMask);

        // record the file-offset of the icon image for when we write the icon directories
        pImageOffset[i] = SetFilePointer(hFile, 0, 0, FILE_CURRENT);

        // bitmapinfoheader + colortable
        if (WriteIconImageHeader(hFile, &bmpColor, &bmpMask) == 0) {
            if (iconInfo.hbmColor)
                DeleteObject(iconInfo.hbmColor);

            if (iconInfo.hbmMask)
                DeleteObject(iconInfo.hbmMask);

            free(pImageOffset);
            CloseHandle(hFile);
            return false;
        }

        // color and mask bitmaps
        if (WriteIconData(hFile, iconInfo.hbmColor) == 0) {
            if (iconInfo.hbmColor)
                DeleteObject(iconInfo.hbmColor);

            if (iconInfo.hbmMask)
                DeleteObject(iconInfo.hbmMask);

            free(pImageOffset);
            CloseHandle(hFile);
            return false;
        }

        if (WriteIconData(hFile, iconInfo.hbmMask) == 0) {
            if (iconInfo.hbmColor)
                DeleteObject(iconInfo.hbmColor);

            if (iconInfo.hbmMask)
                DeleteObject(iconInfo.hbmMask);

            free(pImageOffset);
            CloseHandle(hFile);
            return false;
        }

        if (iconInfo.hbmColor)
            DeleteObject(iconInfo.hbmColor);

        if (iconInfo.hbmMask)
            DeleteObject(iconInfo.hbmMask);
    }

    SetFilePointer(hFile, sizeof(ICONHEADER), 0, FILE_BEGIN);

    for (size_t i = 0; i < hIcons.size(); i++) {
        if (WriteIconDirectoryEntry(hFile, i, hIcons[i], pImageOffset[i]) == 0) {
            free(pImageOffset);
            CloseHandle(hFile);
            return false;
        }
    }

    free(pImageOffset);
    CloseHandle(hFile);

    return true;
}

}  // namespace ashe
#endif  // ASHE_WIN