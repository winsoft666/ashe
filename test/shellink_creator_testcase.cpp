#include "catch.hpp"
#include "ashe/filesystem.hpp"
#include "ashe/path_util.h"
#include "ashe/shellink_creator.h"

#ifdef ASHE_WIN
TEST_CASE("ShellinkCreator", "") {
    std::wstring lnkPath = ashe::PathUtil::GetCurrentUserDesktopFolder() + L"test.lnk";

    std::error_code ec;
    if (ashe::fs::exists(lnkPath, ec)) {
        ashe::fs::remove(lnkPath);
    }

    ashe::ShellinkCreator::ShellLinkProperties prop;
    prop.setTarget(L"C:\\abc\\a.exe");
    prop.setWorkingDir(L"C:\\abc\\");

    bool ret = ashe::ShellinkCreator::CreateShellLink(lnkPath,
                                                         prop,
                                                         ashe::ShellinkCreator::OperationOption::SHORTCUT_UPDATE_EXISTING);
    REQUIRE(!ret);

    ret = ashe::ShellinkCreator::CreateShellLink(lnkPath,
                                                    prop,
                                                    ashe::ShellinkCreator::OperationOption::SHORTCUT_CREATE_ALWAYS);
    REQUIRE(ret);

    prop.setTarget(L"C:\\abcd\\b.exe");
    prop.setWorkingDir(L"C:\\abcd\\");
    ret = ashe::ShellinkCreator::CreateShellLink(lnkPath,
                                                    prop,
                                                    ashe::ShellinkCreator::OperationOption::SHORTCUT_CREATE_ALWAYS);
    REQUIRE(ret);

    REQUIRE(ashe::fs::exists(lnkPath, ec));
    REQUIRE(ashe::fs::remove(lnkPath, ec));
}

#endif