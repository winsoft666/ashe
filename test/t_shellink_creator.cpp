#include "catch.hpp"
#include "ashe/path_util.h"
#include "ashe/win_shellink_creator.h"

#if 0
#ifdef ASHE_WIN
TEST_CASE("ShellinkCreator", "") {
    std::wstring lnkPath = ashe::GetCurrentUserDesktopFolder() + L"test.lnk";

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
}

#endif
#endif