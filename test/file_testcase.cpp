#include "catch.hpp"
#include "ashe/file.h"

// Test: write/read big file.
//
TEST_CASE("FileTest1", "[w/r big file]") {
    constexpr int64_t bytes4gb = 4LL * 1024LL * 1024LL * 1024LL;
    const std::string str1K(1024, 'a');

    ashe::fs::path path1(u8"__file_test_文件测试1__.dat");
    ashe::fs::path openMode1(u8"ab+");

    ashe::File file1(path1);
    REQUIRE(file1.path() == path1);
    if (ashe::fs::exists(file1.path()))
        REQUIRE(ashe::fs::remove(file1.path()));

    REQUIRE(ashe::fs::exists(file1.path()) == false);
    REQUIRE(file1.isOpen() == false);
    REQUIRE(file1.exist() == false);
    REQUIRE(file1.open(openMode1));

    // write 4GB file
    for (size_t i = 0; i < 4 * 1024 * 1024; i++) {
        file1.writeFrom(str1K.c_str(), str1K.size());
    }

    REQUIRE(file1.flush());
    REQUIRE(file1.fileSize() == bytes4gb);

    std::string strRead(1024, '\0');
    REQUIRE(file1.readFrom((void*)&strRead[0], 1024, 0) == 1024);
    REQUIRE(strRead == str1K);

    std::string strRead2(1024, '\0');
    REQUIRE(file1.readFrom((void*)&strRead2[0], 1024, 3LL * 1024LL * 1024LL * 1024LL) == 1024);
    REQUIRE(strRead2 == str1K);

    REQUIRE(file1.fileSize() == bytes4gb);
    REQUIRE(file1.close());
    REQUIRE(ashe::fs::remove(file1.path()));
}

// Test: read all of file content
TEST_CASE("FileTest2", "[read all content]") {
    constexpr int64_t bytes4mb = 4LL * 1024LL * 1024LL;
    const std::string str1K(1024, 'a');
    ashe::fs::path path2(u8"__file_test_文件测试2__.dat");
    if (ashe::fs::exists(path2))
        REQUIRE(ashe::fs::remove(path2));

    ashe::File file2(path2);
    REQUIRE(file2.open("ab+"));

    // write 4MB file
    for (size_t i = 0; i < 4 * 1024; i++) {
        file2.writeFrom(str1K.c_str(), str1K.size());
    }

    REQUIRE(file2.flush());
    REQUIRE(file2.fileSize() == bytes4mb);
    std::string strAll;
    REQUIRE(file2.readAll(strAll));
    REQUIRE(strAll.size() == bytes4mb);

    REQUIRE(file2.isOpen());
    REQUIRE(file2.close());

    REQUIRE(ashe::fs::exists(path2));
    REQUIRE(ashe::fs::remove(path2));
}
