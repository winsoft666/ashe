#include "catch.hpp"
#include "ashe/win_shellink_parser.hpp"

#ifdef ASHE_WIN
TEST_CASE("shellink-parse", "") {
    std::wstring lnkPath = LR"()";

    if (!lnkPath.empty()) {
        ashe::WinShellinkParser wsl;
        ashe::WinShellinkParser::ShellinkErr err = wsl.load(lnkPath);
        CHECK(err == ashe::WinShellinkParser::ShellinkErr::SHLLINK_ERR_NONE);

        std::wstring displayName = wsl.getDescription();
        CHECK(displayName == L"");

        std::wstring targetPath = wsl.getTargetPath();
        CHECK(targetPath == LR"()");

        std::wstring iconPath = wsl.getIconPath();
        CHECK(iconPath == LR"()");

        bool isRunAsAdmin = wsl.isRunAsAdmin();
        CHECK(isRunAsAdmin == false);
    }
}

#endif