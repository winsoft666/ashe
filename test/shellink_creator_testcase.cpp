#include "catch.hpp"
#include "ashe/path_util.hpp"
#include "ashe/win_shellink_creator.hpp"

#ifdef ASHE_WIN
TEST_CASE("ShellinkCreator", "1") {
    ashe::WinShellinkCreator::ShellLinkProperties prop;
    prop.setTarget(L"C:\\a.exe");
    bool ret = ashe::WinShellinkCreator::CreateShellLink(ashe::PathUtil::GetCurrentUserDesktopFolder() + L"test.lnk",
                                                        prop,
                                                        ashe::WinShellinkCreator::OperationOption::SHORTCUT_CREATE_ALWAYS);
    CHECK(ret);
}

#endif