#include "catch.hpp"
#include "ashe/all.hpp"

#ifdef ASHE_WIN

TEST_CASE("IconTest-load-from-current-resource", "") {
    using namespace ashe;

    HICON hIcon = WinIcon::LoadFromProcessRes(GetModuleHandleW(NULL), MAKEINTRESOURCE(101));
    CHECK(hIcon != NULL);
    CHECK(WinIcon::SaveToFile({hIcon}, L"current_exe_output.ico"));
    CHECK(DestroyIcon(hIcon));
}

TEST_CASE("IconTest-load-from-exe", "") {
    using namespace ashe;
    std::wstring exePath = LR"()";

    if (!exePath.empty()) {
        HICON hIcon = WinIcon::LoadFromProcessRes(exePath, MAKEINTRESOURCE(101));
        CHECK(hIcon != NULL);
        CHECK(WinIcon::SaveToFile({hIcon}, L"output.ico"));
        CHECK(DestroyIcon(hIcon));
    }
}

TEST_CASE("IconTest-enum-icons", "") {
    using namespace ashe;
    std::wstring exePath = LR"()";

    if (!exePath.empty()) {
        std::vector<WinIcon::IconGroup> iconGroups;
        CHECK(WinIcon::EnumIconGroups(exePath, iconGroups));

        for (size_t i = 0; i < iconGroups.size(); i++) {
            CHECK(WinIcon::SaveToFile(iconGroups[i].iconHandleList(), std::to_wstring(i) + L".ico"));
        }
    }
}

TEST_CASE("IconTest-get-exe-icon", "") {
    using namespace ashe;
    int actualSize = 0;

    std::wstring exePath = LR"()";

    if (!exePath.empty()) {
        HICON hIcon1 = WinIcon::GetExeIcon(exePath, 128, actualSize);
        CHECK(hIcon1);
        CHECK(actualSize == 128);
        CHECK(WinIcon::SaveToFile({hIcon1}, L"exe_128.ico"));
        CHECK(DestroyIcon(hIcon1));

        HICON hIcon2 = WinIcon::GetExeIcon(exePath, 64, actualSize);
        CHECK(hIcon2);
        CHECK(actualSize == 64);
        CHECK(WinIcon::SaveToFile({hIcon2}, L"exe_64.ico"));
        CHECK(DestroyIcon(hIcon2));

        HICON hIcon3 = WinIcon::GetExeIcon(exePath, 35, actualSize);
        CHECK(hIcon3);
        CHECK(actualSize == 32);
        CHECK(WinIcon::SaveToFile({hIcon3}, L"exe_35.ico"));
        CHECK(DestroyIcon(hIcon3));

        HICON hIcon4 = WinIcon::GetExeIcon(exePath, 98, actualSize);
        CHECK(hIcon4);
        CHECK(actualSize == 128);
        CHECK(WinIcon::SaveToFile({hIcon4}, L"exe_98.ico"));
        CHECK(DestroyIcon(hIcon4));
    }
}
#endif