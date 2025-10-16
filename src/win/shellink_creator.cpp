#include "ashe/config.h"
#include "ashe/win/shellink_creator.h"
#include "ashe/win/windows_lite.h"
#include <shellapi.h>
#include <shlobj.h>
#include <propkey.h>
#include <strsafe.h>
#include "ashe/os_version.h"
#include <assert.h>
#include "ashe/file.h"
#include "ashe/scoped_object.h"

namespace ashe {
namespace win {
namespace {
HRESULT InitializeShortcutInterfaces(const wchar_t* shortcut,
                                     IShellLink** i_shell_link,
                                     IPersistFile** i_persist_file) {
    if ((*i_shell_link)) {
        (*i_shell_link)->Release();
        *i_shell_link = NULL;
    }
    if ((*i_persist_file)) {
        (*i_persist_file)->Release();
        *i_persist_file = NULL;
    }

    HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink,
                                    (void**)i_shell_link);
    if (FAILED(hres)) {
        return hres;
    }

    hres = (*i_shell_link)->QueryInterface(IID_IPersistFile, (void**)i_persist_file);
    if (FAILED(hres)) {
        (*i_shell_link)->Release();
        *i_shell_link = NULL;
        return hres;
    }

    if (shortcut) {
        hres = (*i_persist_file)->Load(shortcut, STGM_READWRITE);
        if (FAILED(hres)) {
            (*i_shell_link)->Release();
            *i_shell_link = NULL;

            (*i_persist_file)->Release();
            *i_persist_file = NULL;

            return hres;
        }
    }

    return S_OK;
}
}  // namespace

bool ShellinkCreator::CreateShellLink(const std::wstring& shellLinkPath,
                                         const ShellLinkProperties& properties,
                                         OperationOption operation) {
    ScopedComInitialize comInit;

    // A target is required unless |operation| is SHORTCUT_UPDATE_EXISTING.
    if (operation != OperationOption::SHORTCUT_UPDATE_EXISTING &&
        !(properties.options & ShellLinkProperties::PROPERTIES_TARGET)) {
        return false;
    }

    bool alreadyExisted = true;
    if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(shellLinkPath.c_str()) && GetLastError() == ERROR_FILE_NOT_FOUND) {
        alreadyExisted = false;
    }

    // Interfaces to the old shortcut when replacing an existing shortcut.
    IShellLink* oldShellLink = NULL;
    IPersistFile* oldPersistFile = NULL;

    // Interfaces to the shortcut being created/updated.
    IShellLink* iShellLink = NULL;
    IPersistFile* iPersistFile = NULL;
    HRESULT hr = E_FAIL;
    switch (operation) {
        case OperationOption::SHORTCUT_CREATE_ALWAYS:
            hr = InitializeShortcutInterfaces(NULL, &iShellLink, &iPersistFile);
            break;
        case OperationOption::SHORTCUT_UPDATE_EXISTING:
            hr = InitializeShortcutInterfaces(shellLinkPath.c_str(), &iShellLink, &iPersistFile);
            break;
        case OperationOption::SHORTCUT_REPLACE_EXISTING:
            hr = InitializeShortcutInterfaces(shellLinkPath.c_str(), &oldShellLink, &oldPersistFile);
            // Confirm |shortcut_path| exists and is a shortcut by verifying
            // |old_i_persist_file| was successfully initialized in the call above. If
            // so, initialize the interfaces to begin writing a new shortcut (to
            // overwrite the current one if successful).
            if (SUCCEEDED(hr) && oldPersistFile)
                hr = InitializeShortcutInterfaces(NULL, &iShellLink, &iPersistFile);
            break;
        default:
            assert(false);
    }

    // Return false immediately upon failure to initialize shortcut interfaces.
    if (FAILED(hr)) {
        return false;
    }

    assert(iPersistFile && iShellLink);
    if (!iPersistFile || !iShellLink)
        return false;

    if ((properties.options & ShellLinkProperties::PROPERTIES_TARGET) &&
        FAILED(iShellLink->SetPath(properties.target.c_str()))) {
        return false;
    }

    if ((properties.options & ShellLinkProperties::PROPERTIES_WORKING_DIR) &&
        FAILED(iShellLink->SetWorkingDirectory(properties.working_dir.c_str()))) {
        return false;
    }

    if (properties.options & ShellLinkProperties::PROPERTIES_ARGUMENTS) {
        if (FAILED(iShellLink->SetArguments(properties.arguments.c_str())))
            return false;
    }
    else if (oldPersistFile) {
        wchar_t current_arguments[MAX_PATH] = {0};
        if (SUCCEEDED(oldShellLink->GetArguments(current_arguments, MAX_PATH))) {
            iShellLink->SetArguments(current_arguments);
        }
    }

    if ((properties.options & ShellLinkProperties::PROPERTIES_DESCRIPTION) &&
        FAILED(iShellLink->SetDescription(properties.description.c_str()))) {
        return false;
    }

    if ((properties.options & ShellLinkProperties::PROPERTIES_ICON) &&
        FAILED(iShellLink->SetIconLocation(properties.icon.c_str(), properties.icon_index))) {
        return false;
    }

    // Release the interfaces to the old shortcut to make sure it doesn't prevent
    // overwriting it if needed.
    if (oldPersistFile)
        oldPersistFile->Release();
    if (oldShellLink)
        oldShellLink->Release();

    HRESULT result = iPersistFile->Save(shellLinkPath.c_str(), TRUE);

    // Release the interfaces in case the SHChangeNotify call below depends on
    // the operations above being fully completed.
    iPersistFile->Release();
    iShellLink->Release();

    // If we successfully created/updated the icon, notify the shell that we have
    // done so.
    const bool succeeded = SUCCEEDED(result);
    if (succeeded) {
        if (alreadyExisted) {
            // TODO(gab): SHCNE_UPDATEITEM might be sufficient here; further testing
            // required.
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
        }
        else {
            SHChangeNotify(SHCNE_CREATE, SHCNF_PATH, shellLinkPath.c_str(), NULL);
        }
    }

    return succeeded;
}

bool ShellinkCreator::ResolveShellLink(const std::wstring& shellLinkPath, ShellLinkProperties& properties) {
    ScopedComInitialize comInit;

    HRESULT result;
    IShellLink* iShellLink = NULL;
    IPersistFile* persist = NULL;

    HRESULT hres = CoCreateInstance(CLSID_ShellLink,
                                    NULL,
                                    CLSCTX_INPROC_SERVER,
                                    IID_IShellLink,
                                    (void**)&iShellLink);
    if (FAILED(hres)) {
        return false;
    }

    hres = iShellLink->QueryInterface(IID_IPersistFile, (void**)&persist);
    if (FAILED(hres)) {
        iShellLink->Release();
        return false;
    }

    if (FAILED(persist->Load(shellLinkPath.c_str(), STGM_READWRITE))) {
        iShellLink->Release();
        persist->Release();
        return false;
    }

    WCHAR temp[MAX_PATH] = {0};
    // Try to find the target of a shortcut.
    result = iShellLink->Resolve(0, SLR_NO_UI | SLR_NOSEARCH);
    if (FAILED(result))
        return false;

    result = iShellLink->GetPath(temp, MAX_PATH, NULL, SLGP_UNCPRIORITY);
    if (FAILED(result))
        return false;
    properties.target = temp;

    result = iShellLink->GetArguments(temp, MAX_PATH);
    if (FAILED(result))
        return false;
    properties.arguments = temp;

    result = iShellLink->GetWorkingDirectory(temp, MAX_PATH);
    if (FAILED(result))
        return false;
    properties.working_dir = temp;

    result = iShellLink->GetDescription(temp, MAX_PATH);
    if (FAILED(result))
        return false;
    properties.description = temp;

    int iconIndex = 0;
    result = iShellLink->GetIconLocation(temp, MAX_PATH, &iconIndex);
    if (FAILED(result))
        return false;
    properties.icon = temp;
    properties.icon_index = iconIndex;

    if (persist)
        persist->Release();

    if (iShellLink)
        iShellLink->Release();

    return true;
}

bool ShellinkCreator::TaskbarPinShellLink(const std::wstring& shellLinkPath) {
    if (!IsWindowsVistaOrHigher())
        return false;

    INT_PTR result = (INT_PTR)(ShellExecuteW(NULL, L"taskbarpin", shellLinkPath.c_str(), NULL, NULL, 0));
    return result > 32;
}

bool ShellinkCreator::TaskbarUnpinShellLink(const std::wstring& shellLinkPath) {
    if (!IsWindowsVistaOrHigher())
        return false;

    INT_PTR result = (INT_PTR)(ShellExecuteW(NULL, L"taskbarunpin", shellLinkPath.c_str(), NULL, NULL, 0));
    return result > 32;
}
}  // namespace win
}  // namespace ashe