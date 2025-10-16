#include "catch.hpp"
#include "ashe/arch.h"
#ifdef ASHE_WIN
#include "ashe/win/machine_uuid.h"

using namespace ashe;
using namespace ashe::win;

TEST_CASE("PersisitentUUIDTest", "") {
    std::wstring uuid;
    uuid = MachineUUID::GetMachineGUID();

    REQUIRE(!uuid.empty());

    std::wstring softUUID = MachineUUID::GetSoftwareUUID(L"UUIDTest");
    REQUIRE(!softUUID.empty());
    for (int i = 0; i < 1000; i++) {
        std::wstring str = MachineUUID::GetSoftwareUUID(L"UUIDTest");
        REQUIRE(softUUID == str);
    }
}
#endif