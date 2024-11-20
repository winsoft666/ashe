#include "catch.hpp"
#include "ashe/string_encode.h"
#include "ashe/base64.h"
#include "ashe/compiler_specific.h"

// Test: string encode, utf8/utf16
//
TEST_CASE("StringEncodeTest") {
#if ASHE_CPP_STANDARD_VER >= 202002L
    std::string u8str = reinterpret_cast<const char*>(u8"中国china");
#else
    const std::string u8str = u8"中国china";
#endif
    const std::wstring wstr = L"中国china";
    REQUIRE(ashe::Utf8ToUnicode(u8str) == wstr);
    REQUIRE(ashe::UnicodeToUtf8(wstr) == u8str);
}

// Test: string base64 encode/decode.
//
TEST_CASE("Base64Test") {
    std::vector<uint8_t> data = {255, 254, 128, 1};
    std::string basedData = ashe::Base64::Encode(&data[0], data.size());
    std::vector<uint8_t> decodedData = ashe::Base64::Decode(basedData);
    REQUIRE(data == decodedData);
#if ASHE_CPP_STANDARD_VER >= 201703L
    REQUIRE(ashe::Base64::Encode(std::string_view("hello world!")) == "aGVsbG8gd29ybGQh");
#else
    REQUIRE(ashe::Base64::Encode("hello world!") == "aGVsbG8gd29ybGQh");
#endif
}