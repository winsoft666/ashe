#include "catch.hpp"
#include "ashe/arch.h"
#ifdef ASHE_WIN
#include "ashe/win/shellink_parser.h"

using namespace ashe::win;

#if 0
TEST_CASE("shellink-parse", "") {
    std::wstring lnkPath = LR"()";

    if (!lnkPath.empty()) {
        ShellinkParser wsl;
        ShellinkParser::ShellinkErr err = wsl.load(lnkPath);
        CHECK(err == ShellinkParser::ShellinkErr::SHLLINK_ERR_NONE);

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