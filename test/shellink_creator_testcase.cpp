#include "catch.hpp"
#include "ashe/filesystem.hpp"
#include "ashe/path_util.hpp"
#include "ashe/win_shellink_creator.hpp"

#ifdef ASHE_WIN
TEST_CASE("ShellinkCreator", "") {
    std::wstring lnkPath = ashe::PathUtil::GetCurrentUserDesktopFolder() + L"test.lnk";

    std::error_code ec;
    if (ashe::fs::exists(lnkPath, ec)) {
        ashe::fs::remove(lnkPath);
    }

    ashe::WinShellinkCreator::ShellLinkProperties prop;
    prop.setTarget(L"C:\\abc\\a.exe");
    prop.setWorkingDir(L"C:\\abc\\");

    bool ret = ashe::WinShellinkCreator::CreateShellLink(lnkPath,
                                                         prop,
                                                         ashe::WinShellinkCreator::OperationOption::SHORTCUT_UPDATE_EXISTING);
    REQUIRE(!ret);

    ret = ashe::WinShellinkCreator::CreateShellLink(lnkPath,
                                                    prop,
                                                    ashe::WinShellinkCreator::OperationOption::SHORTCUT_CREATE_ALWAYS);
    REQUIRE(ret);

    prop.setTarget(L"C:\\abcd\\b.exe");
    prop.setWorkingDir(L"C:\\abcd\\");
    ret = ashe::WinShellinkCreator::CreateShellLink(lnkPath,
                                                    prop,
                                                    ashe::WinShellinkCreator::OperationOption::SHORTCUT_CREATE_ALWAYS);
    REQUIRE(ret);

    REQUIRE(ashe::fs::exists(lnkPath, ec));
    REQUIRE(ashe::fs::remove(lnkPath, ec));
}

#endif