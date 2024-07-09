#include "catch.hpp"
#include "ashe/win_registry.h"

#ifdef ASHE_WIN
TEST_CASE("winreg-open-not-exist-1", "") {
    using namespace ashe;
    WinRegistry reg(HKEY_CURRENT_USER, L"Software\\ashe-not-exist");
    REQUIRE(ERROR_SUCCESS != (reg.open(KEY_ALL_ACCESS, false)));
    reg.close();
}

TEST_CASE("WinReg-open-not-exist-2", "") {
    using namespace ashe;
    WinRegistry reg(HKEY_CURRENT_USER, L"Software\\ashe-not-exist");
    REQUIRE(ERROR_SUCCESS != (reg.open(KEY_ALL_ACCESS, false)));
    reg.close();
}

TEST_CASE("WinReg-get-not-exist-dword", "") {
    using namespace ashe;
    WinRegistry reg(HKEY_CURRENT_USER, L"Software\\ashe-not-exist");
    REQUIRE(ERROR_SUCCESS != (reg.open(KEY_ALL_ACCESS, false)));

    DWORD dwValue = 0;
    REQUIRE(ERROR_SUCCESS != (reg.getDWORDValue(L"not-exist", dwValue)));
    REQUIRE(dwValue == 0);

    reg.close();
}

TEST_CASE("WinReg-get-not-exist-sz", "") {
    using namespace ashe;
    WinRegistry reg(HKEY_CURRENT_USER, L"Software\\ashe-not-exist");
    REQUIRE(ERROR_SUCCESS != (reg.open(KEY_ALL_ACCESS, false)));

    std::wstring strValue;
    REQUIRE(ERROR_SUCCESS != (reg.getSZValue(L"not-exist", strValue)));
    REQUIRE(strValue == L"");

    reg.close();
}

TEST_CASE("WinReg-get-not-exist-bin", "") {
    using namespace ashe;
    WinRegistry reg(HKEY_CURRENT_USER, L"Software\\ashe-not-exist");
    REQUIRE(ERROR_SUCCESS != (reg.open(KEY_ALL_ACCESS, false)));

    BYTE szData[1024] = {0};
    REQUIRE(ERROR_SUCCESS != (reg.getBINARYValue(L"not-exist", szData, 1024)));
    REQUIRE(szData[0] == 0);

    reg.close();
}

TEST_CASE("WinReg-create-rw-del", "") {
    using namespace ashe;
    LSTATUS ls = WinRegistry::DeleteSubKeys(HKEY_CURRENT_USER, L"Software\\3A698285-E40A-44BC-9FB1-600DF02488DD", false);
    REQUIRE((ls == ERROR_SUCCESS || ls == ERROR_FILE_NOT_FOUND));

    WinRegistry reg(HKEY_CURRENT_USER, L"Software\\3A698285-E40A-44BC-9FB1-600DF02488DD");
    REQUIRE(ERROR_SUCCESS == reg.open(KEY_ALL_ACCESS, true));
    REQUIRE(reg.isOpen());

    REQUIRE(ERROR_SUCCESS == (reg.open(KEY_ALL_ACCESS, false)));

    reg.close();

    REQUIRE(ERROR_SUCCESS == (reg.open(KEY_ALL_ACCESS, false)));

    REQUIRE(ERROR_SUCCESS == (reg.setDWORDValue(L"a", 100)));
    REQUIRE(ERROR_SUCCESS == (reg.setDWORDValue(L"a", 200)));
    REQUIRE(ERROR_SUCCESS == (reg.setDWORDValue(L"b", 300)));
    REQUIRE(ERROR_SUCCESS == (reg.setSZValue(L"s1", L"hello")));
    std::wstring str2048;
    for (int i = 0; i < 2048; i++) {
        str2048 += L"d";
    }
    REQUIRE(ERROR_SUCCESS == (reg.setSZValue(L"s2048", str2048)));

    std::wstring strSZ;
    REQUIRE(ERROR_SUCCESS == (reg.getSZValue(L"s2048", strSZ)));
    REQUIRE(strSZ == str2048);

    REQUIRE(ERROR_SUCCESS == (reg.setExpandSZValue(L"s2048", str2048)));
    REQUIRE(ERROR_SUCCESS != (reg.getSZValue(L"s2048", strSZ)));

    std::vector<std::wstring> subKeys;
    REQUIRE(ERROR_SUCCESS == (reg.getSubKeys(subKeys)));
    REQUIRE(subKeys.size() == 0);

    REQUIRE(ERROR_SUCCESS == WinRegistry::DeleteSubKeys(HKEY_CURRENT_USER, L"Software\\3A698285-E40A-44BC-9FB1-600DF02488DD", false));

    reg.close();
    REQUIRE(ERROR_SUCCESS != (reg.open(KEY_ALL_ACCESS, false)));
}

TEST_CASE("WinReg-enum-subkey", "") {
    using namespace ashe;
    WinRegistry reg(HKEY_CURRENT_USER, L"Software\\3A698285-E40A-44BC-9FB1-600DF02488DD\\1\\2\\3\\4\\5");
    REQUIRE(ERROR_SUCCESS == (reg.open(KEY_ALL_ACCESS, true)));
    REQUIRE(reg.isOpen());

    std::vector<std::wstring> subKeys;
    REQUIRE(ERROR_SUCCESS == (reg.getSubKeys(subKeys)));
    REQUIRE(subKeys.size() == 0);

    reg.close();

    WinRegistry reg2(HKEY_CURRENT_USER, L"Software\\3A698285-E40A-44BC-9FB1-600DF02488DD");
    REQUIRE(ERROR_SUCCESS == (reg2.open(KEY_ALL_ACCESS, false)));

    REQUIRE(ERROR_SUCCESS == (reg2.getSubKeys(subKeys)));
    REQUIRE(subKeys.size() == 1);
    REQUIRE(subKeys[0] == L"1");

    for (int i = 0; i < 10; i++) {
        WinRegistry reg(HKEY_CURRENT_USER, L"Software\\3A698285-E40A-44BC-9FB1-600DF02488DD\\" + std::to_wstring(i));
        REQUIRE(ERROR_SUCCESS == (reg.open(KEY_ALL_ACCESS, true)));
    }

    WinRegistry reg3(HKEY_CURRENT_USER, L"Software\\3A698285-E40A-44BC-9FB1-600DF02488DD");
    REQUIRE(ERROR_SUCCESS == (reg3.open(KEY_ALL_ACCESS, false)));
    REQUIRE(ERROR_SUCCESS == (reg3.getSubKeys(subKeys)));
    REQUIRE(subKeys.size() == 10);
}

TEST_CASE("WinReg-multi-sz", "") {
    using namespace ashe;
    WinRegistry reg(HKEY_CURRENT_USER, L"Software\\3A698285-E40A-44BC-9FB1-600DF02488DD");
    REQUIRE(ERROR_SUCCESS == (reg.open(KEY_ALL_ACCESS, true)));

    std::vector<std::wstring> ss;
    for (int i = 0; i < 100; i++) {
        ss.push_back(L"aaaa" + std::to_wstring(i));
    }

    REQUIRE(ERROR_SUCCESS == (reg.setMultiSZValue(L"multi-sz", ss)));

    std::vector<std::wstring> rr;
    REQUIRE(ERROR_SUCCESS == (reg.getMultiSZValue(L"multi-sz", rr)));

    REQUIRE(ss == rr);

    REQUIRE(ERROR_SUCCESS == WinRegistry::DeleteSubKeys(HKEY_CURRENT_USER, L"Software\\3A698285-E40A-44BC-9FB1-600DF02488DD", false));
}

#endif