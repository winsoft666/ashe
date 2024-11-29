#include "ashe/win/persistent_uuid.h"
#include "ashe/win/registry.h"
#include "ashe/time_util.h"
#include "ashe/uuid.h"
#include "ashe/string_encode.h"
#include "ashe/logging.h"

namespace ashe {
namespace win {
bool GetPersistentUUID(std::wstring& result) {
    std::wstring str;
    Registry reg(HKEY_CURRENT_USER, L"166258986303612071201");
    if (ERROR_SUCCESS != reg.open(KEY_ALL_ACCESS | KEY_WOW64_32KEY, true)) {
        return false;
    }

    if (reg.getSZValue(L"Value", str) == ERROR_SUCCESS && !str.empty()) {
        result = str;
        return true;
    }

    str = u2w(UUID::Create());
    if (str.empty()) {
        str = std::to_wstring(GetCurrentTimestampByMicroSec());
    }

    DCHECK(!str.empty());
    if (str.empty()) {
        return false;
    }

    if (reg.setSZValue(L"Value", str) != ERROR_SUCCESS) {
        return false;
    }

    result = str;

    return true;
}
}  // namespace win
}  // namespace ashe