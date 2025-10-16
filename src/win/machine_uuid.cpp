#include "ashe/win/machine_uuid.h"
#include "ashe/win/registry.h"
#include "ashe/time_util.h"
#include "ashe/uuid.h"
#include "ashe/string_encode.h"
#include "ashe/logging.h"

namespace ashe {
namespace win {
std::wstring MachineUUID::GetSoftwareUUID(const std::wstring& softwareKey) {
    if (softwareKey.empty()) {
        return L"";
    }

    std::wstring str;
    Registry reg(HKEY_CURRENT_USER, softwareKey);
    if (ERROR_SUCCESS != reg.open(KEY_ALL_ACCESS | KEY_WOW64_32KEY, true)) {
        return L"";
    }

    if (reg.getSZValue(L"Value", str) == ERROR_SUCCESS && !str.empty()) {
        return str;
    }

    str = u2w(CreateUuid());
    if (str.empty()) {
        str = std::to_wstring(GetCurrentTimestampByMicroSec());
    }

    DCHECK(!str.empty());
    if (str.empty()) {
        return L"";
    }

    if (reg.setSZValue(L"Value", str) != ERROR_SUCCESS) {
        return L"";
    }

    return str;
}

std::wstring MachineUUID::GetMachineGUID() {
    Registry reg(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography");
    if (ERROR_SUCCESS != reg.open(KEY_QUERY_VALUE, true)) {
        return L"";
    }

    std::wstring str;
    if (reg.getSZValue(L"MachineGuid", str) != ERROR_SUCCESS) {
        return L"";
    }

    return str;
}

}  // namespace win
}  // namespace ashe