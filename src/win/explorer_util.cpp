#include "ashe/win/explorer_util.h"
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif  // !_WINSOCKAPI_
#include <shlobj_core.h>
#include <shellapi.h>
#include <Shlwapi.h>

namespace ashe {
namespace win {
bool OpenWinExplorerAndLocate(const std::wstring& path) {
    bool result = false;
    PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(path.c_str());
    if (pidl) {
        result = (S_OK == SHOpenFolderAndSelectItems(pidl, 0, NULL, 0));
        ILFree(pidl);
    }
    return result;
}

bool GetWinExplorerDisplayName(const std::wstring& path, std::wstring& displayName) {
    SHFILEINFOW sfi;
    ZeroMemory(&sfi, sizeof(SHFILEINFOW));
    DWORD_PTR dwRet = ::SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFOW), SHGFI_DISPLAYNAME);
    if (dwRet == 0) {
        return false;
    }
    displayName = sfi.szDisplayName;
    return true;
}
}  // namespace win
}  // namespace ashe