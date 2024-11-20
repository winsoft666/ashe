#include "catch.hpp"
#include "ashe/arch.h"

#ifdef ASHE_WIN
#include "ashe/path_util.h"
#include "ashe/win/shellink_creator.h"

using namespace ashe;
using namespace ashe::win;

#if 0
TEST_CASE("ShellinkCreator", "") {
    std::wstring lnkPath = GetCurrentUserDesktopFolder() + L"test.lnk";

    ShellinkCreator::ShellLinkProperties prop;
    prop.setTarget(L"C:\\abc\\a.exe");
    prop.setWorkingDir(L"C:\\abc\\");

    bool ret = ShellinkCreator::CreateShellLink(lnkPath,
                                                         prop,
                                                         ShellinkCreator::OperationOption::SHORTCUT_UPDATE_EXISTING);
    REQUIRE(!ret);

    ret = ShellinkCreator::CreateShellLink(lnkPath,
                                                    prop,
                                                    ShellinkCreator::OperationOption::SHORTCUT_CREATE_ALWAYS);
    REQUIRE(ret);

    prop.setTarget(L"C:\\abcd\\b.exe");
    prop.setWorkingDir(L"C:\\abcd\\");
    ret = ShellinkCreator::CreateShellLink(lnkPath,
                                                    prop,
                                                    ShellinkCreator::OperationOption::SHORTCUT_CREATE_ALWAYS);
    REQUIRE(ret);
}

#endif
#endif