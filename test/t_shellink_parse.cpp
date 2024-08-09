#include "catch.hpp"
#include "ashe/shellink_parser.h"

#if 0
#ifdef ASHE_WIN
TEST_CASE("shellink-parse", "") {
    std::wstring lnkPath = LR"()";

    if (!lnkPath.empty()) {
        ashe::ShellinkParser wsl;
        ashe::ShellinkParser::ShellinkErr err = wsl.load(lnkPath);
        CHECK(err == ashe::ShellinkParser::ShellinkErr::SHLLINK_ERR_NONE);

        std::wstring displayName = wsl.getDescription();
        CHECK(displayName == L"");

        std::wstring targetPath = wsl.getTargetPath();
        CHECK(targetPath == LR"()");

        std::wstring iconPath = wsl.getIconPath();
        CHECK(iconPath == LR"()");

        int32_t iconIndex = wsl.getIconIndex();
        CHECK(iconIndex == 0);

        bool isRunAsAdmin = wsl.isRunAsAdmin();
        CHECK(isRunAsAdmin == false);
    }
}

#endif
#endif