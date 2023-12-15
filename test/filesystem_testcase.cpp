#include "catch.hpp"
#include "ashe/all.hpp"

// Test: path attribute getter/convert
//
TEST_CASE("FileSystemTest1", "[path attribute]") {
#ifdef ASHE_WIN
    ashe::fs::path path1(u8"C:/testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.wstring() == L"C:\\testoi89hk8\\abc\\__filesystem_test1__.dat");
    REQUIRE(path1.generic_wstring() == L"C:/testoi89hk8/abc/__filesystem_test1__.dat");

    REQUIRE(path1.string() == u8"C:\\testoi89hk8\\abc\\__filesystem_test1__.dat");
    REQUIRE(path1.generic_string() == u8"C:/testoi89hk8/abc/__filesystem_test1__.dat");

    REQUIRE(path1.has_extension());
    REQUIRE(path1.has_filename());
    REQUIRE(path1.has_parent_path());
    REQUIRE(path1.has_relative_path());
    REQUIRE(path1.has_root_directory());
    REQUIRE(path1.has_root_name());
    REQUIRE(path1.has_root_path());
    REQUIRE(path1.has_stem());

    REQUIRE(path1.extension().u8string() == u8".dat");
    REQUIRE(path1.filename() == u8"__filesystem_test1__.dat");
    REQUIRE(path1.stem() == u8"__filesystem_test1__");

    REQUIRE(path1.parent_path().wstring() == L"C:\\testoi89hk8\\abc");
    REQUIRE(path1.parent_path().generic_wstring() == L"C:/testoi89hk8/abc");

    REQUIRE(path1.relative_path().wstring() == L"testoi89hk8\\abc\\__filesystem_test1__.dat");
    REQUIRE(path1.root_name() == "C:");
    REQUIRE(path1.root_directory() == "\\");
    REQUIRE(path1.root_path() == "C:\\");

    path1.replace_filename(L"abctest.txt");
    REQUIRE(path1.string() == u8"C:\\testoi89hk8\\abc\\abctest.txt");

    path1.replace_extension(".txt");
    REQUIRE(path1.string() == u8"C:\\testoi89hk8\\abc\\abctest.txt");

    path1.replace_filename(u8"__filesystem_test1__.dat");
    REQUIRE(path1.generic_string() == u8"C:/testoi89hk8/abc/__filesystem_test1__.dat");

    path1 = L"C:\\testoi89hk8\\..\\123\\.\\__filesystem_test1__.dat";
    ashe::fs::path path2 = ashe::fs::absolute(path1);
    REQUIRE(path2.wstring() == L"C:\\123\\__filesystem_test1__.dat");
#else
    ashe::fs::path path1(u8"./testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.wstring() == L"./testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.generic_wstring() == L"./testoi89hk8/abc/__filesystem_test1__.dat");

    REQUIRE(path1.string() == u8"./testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.generic_string() == u8"./testoi89hk8/abc/__filesystem_test1__.dat");

    REQUIRE(path1.has_extension());
    REQUIRE(path1.has_filename());
    REQUIRE(path1.has_stem());

    REQUIRE(path1.extension().u8string() == u8".dat");
    REQUIRE(path1.filename() == u8"__filesystem_test1__.dat");
    REQUIRE(path1.stem() == u8"__filesystem_test1__");

    REQUIRE(path1.parent_path().wstring() == L"./testoi89hk8/abc");
    REQUIRE(path1.parent_path().generic_wstring() == L"./testoi89hk8/abc");

    std::cout << std::endl;
    std::cout << "relative_path:" << path1.relative_path().string() << std::endl;
    std::cout << "root_name:" << path1.root_name().string() << std::endl;
    std::cout << "root_directory:" << path1.root_directory().string() << std::endl;
    std::cout << "root_path:" << path1.root_path().string() << std::endl;

    REQUIRE(path1.relative_path().string() == "./testoi89hk8/abc/__filesystem_test1__.dat");
    REQUIRE(path1.root_name() == "");
    REQUIRE(path1.root_directory() == "");
    REQUIRE(path1.root_path() == "");

    path1.replace_filename(L"abctest.txt");
    REQUIRE(path1.string() == u8"./testoi89hk8/abc/abctest.txt");

    path1.replace_extension(".txt");
    REQUIRE(path1.string() == u8"./testoi89hk8/abc/abctest.txt");

    path1.replace_filename(u8"__filesystem_test1__.dat");
    REQUIRE(path1.generic_string() == u8"./testoi89hk8/abc/__filesystem_test1__.dat");

    ashe::fs::path path2 = ashe::fs::absolute(path1);
    std::wcout << "absolute:" << path2.wstring() << std::endl;
    REQUIRE(path2.wstring().length() > 4);
#endif
}

// Test: remove empty directory.
//
TEST_CASE("FileSystemTest2", "[remove empty dir]") {
#ifdef ASHE_WIN
    std::error_code ec;
    ashe::fs::path path2(L"C:\\testuy763e\\abc\\__filesystem_test2_" + std::to_wstring(time(nullptr)));
    REQUIRE(ashe::fs::exists(path2, ec) == false);
    REQUIRE(ashe::fs::create_directories(path2, ec));
    REQUIRE(ashe::fs::exists(path2, ec) == true);
    REQUIRE(ashe::fs::remove(path2, ec) == true);
#else
    std::error_code ec;
    ashe::fs::path path2("./testuy763e/abc/__filesystem_test2_" + std::to_string(time(nullptr)));
    REQUIRE(ashe::fs::exists(path2, ec) == false);
    REQUIRE(ashe::fs::create_directories(path2, ec));
    REQUIRE(ashe::fs::exists(path2, ec) == true);
    REQUIRE(ashe::fs::remove(path2, ec) == true);
#endif
}

// Test: remove non-empty directory.
//
TEST_CASE("FileSystemTest3", "[remove non-empty dir]") {
#ifdef ASHE_WIN
    std::error_code ec;
    ashe::fs::path path3(L"C:\\testuy763e\\abc\\__filesystem_test3_" + std::to_wstring(time(nullptr)));
    REQUIRE(ashe::fs::exists(path3, ec) == false);
    REQUIRE(ashe::fs::create_directories(path3, ec));
    REQUIRE(ashe::fs::exists(path3, ec) == true);

    const std::string strWritten = "hello world";

    ashe::fs::path path4 = path3;
    path4.append("path4.txt");

    ashe::File file4(path4);
    REQUIRE(file4.open("wb") == true);
    REQUIRE(file4.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(file4.close());

    REQUIRE(ashe::fs::remove(path3, ec) == false);  // remove can only delete empty directory
    REQUIRE(ashe::fs::remove_all(path3, ec) == 2);  // return remove item count
    REQUIRE(ashe::fs::exists(path3, ec) == false);
    REQUIRE(ashe::fs::exists(path3.parent_path(), ec) == true);
#else
    std::error_code ec;
    ashe::fs::path path3(L"./testuy763e/abc/__filesystem_test3_" + std::to_wstring(time(nullptr)));
    REQUIRE(ashe::fs::exists(path3, ec) == false);
    REQUIRE(ashe::fs::create_directories(path3, ec));
    REQUIRE(ashe::fs::exists(path3, ec) == true);

    const std::string strWritten = "hello world";

    ashe::fs::path path4 = path3;
    path4.append("path4.txt");

    ashe::File file4(path4);
    REQUIRE(file4.open("wb") == true);
    REQUIRE(file4.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(file4.close());

    REQUIRE(ashe::fs::remove(path3, ec) == false);  // remove can only delete empty directory
    REQUIRE(ashe::fs::remove_all(path3, ec) == 2);  // return remove item count
    REQUIRE(ashe::fs::exists(path3, ec) == false);

    REQUIRE(ashe::fs::exists(path3.parent_path(), ec) == true);
#endif
}

// Test: Recursive remove non-empty directory.
//
TEST_CASE("FileSystemTest4", "[recursive remove non-empty attribute]") {
#ifdef ASHE_WIN
    std::error_code ec;
    ashe::fs::path p1(L"C:\\test87w232\\abc\\__filesystem_test4_" + std::to_wstring(time(nullptr)));
    REQUIRE(ashe::fs::exists(p1, ec) == false);
    REQUIRE(ashe::fs::create_directories(p1, ec));
    REQUIRE(ashe::fs::exists(p1, ec) == true);

    const std::string strWritten = "hello world";

    // create C:\test\abc\__filesystem_test4_xxx\p2.txt
    ashe::fs::path p2 = p1;
    p2.append("p2.txt");

    ashe::File f2(p2);
    REQUIRE(f2.open("wb") == true);
    REQUIRE(f2.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f2.close());

    // create C:\test\abc\p3.txt
    ashe::fs::path p3 = p1;
    p3.append("..\\p2.txt");

    ashe::File f3(p3);
    REQUIRE(f3.open("wb") == true);
    REQUIRE(f3.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f3.close());

    // create C:\test\p4.txt
    ashe::fs::path p4 = p1;
    p4.append("..\\..\\p4.txt");

    ashe::File f4(p4);
    REQUIRE(f4.open("wb") == true);
    REQUIRE(f4.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f4.close());

    REQUIRE(ashe::fs::remove(L"C:\\test87w232", ec) == false);  // remove can only delete empty directory
    REQUIRE(ashe::fs::remove_all(L"C:\\test87w232", ec) == 6);  // return remove item count
    REQUIRE(ashe::fs::exists(L"C:\\test87w232", ec) == false);
#else
    std::error_code ec;
    ashe::fs::path p1("~/test87w232/abc/__filesystem_test4_" + std::to_string(time(nullptr)));
    REQUIRE(ashe::fs::exists(p1, ec) == false);
    REQUIRE(ashe::fs::create_directories(p1, ec));
    REQUIRE(ashe::fs::exists(p1, ec) == true);

    const std::string strWritten = "hello world";

    // create ~/test87w232/abc/__filesystem_test4_xxx/p2.txt
    ashe::fs::path p2 = p1;
    p2.append("p2.txt");

    ashe::File f2(p2);
    REQUIRE(f2.open("wb") == true);
    REQUIRE(f2.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f2.close());

    // create ~/test87w232/abc/p3.txt
    ashe::fs::path p3 = p1;
    p3.append("../p2.txt");

    ashe::File f3(p3);
    REQUIRE(f3.open("wb") == true);
    REQUIRE(f3.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f3.close());

    // create ~/test87w232/p4.txt
    ashe::fs::path p4 = p1;
    p4.append("../../p4.txt");

    ashe::File f4(p4);
    REQUIRE(f4.open("wb") == true);
    REQUIRE(f4.writeFrom((void*)strWritten.c_str(), strWritten.size(), 0) == strWritten.size());
    REQUIRE(f4.close());

    REQUIRE(ashe::fs::remove("~/test87w232", ec) == false);  // remove can only delete empty directory
    REQUIRE(ashe::fs::remove_all("~/test87w232", ec) == 6);  // return remove item count
    REQUIRE(ashe::fs::exists("~/test87w232", ec) == false);
#endif
}

TEST_CASE("FileSystemTest5", "path operate") {
    ashe::fs::path p = L"C:\\Program Files(x86)\\a \\c.txt";
    p.remove_filename();
    REQUIRE(p == L"C:\\Program Files(x86)\\a \\");

    std::wstring path = ashe::PathUtil::PathRemoveQuote(std::wstring(L"'C:\\Program Files(x86)\\a \\c.txt'"));
    ashe::fs::path p2 = path;
    p2.remove_filename();
    REQUIRE(p2 == L"C:\\Program Files(x86)\\a \\");
}
