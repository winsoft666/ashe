#include "catch.hpp"
#include "ashe/all.hpp"

// Test: string encode, utf8/utf16
//
TEST_CASE("StringEncodeTest") {
    const std::string u8str = u8"中国china";
    const std::wstring wstr = L"中国china";
    REQUIRE(ashe::StringEncode::Utf8ToUnicode(u8str) == wstr);
    REQUIRE(ashe::StringEncode::UnicodeToUtf8(wstr) == u8str);
}

// Test: string base64 encode/decode.
//
TEST_CASE("Base64Test") {
    std::vector<uint8_t> data = {255, 254, 128, 1};
    std::string basedData = ashe::Base64::Encode(&data[0], data.size());
    std::vector<uint8_t> decodedData = ashe::Base64::Decode(basedData);
    REQUIRE(data == decodedData);
    REQUIRE(ashe::Base64::Encode("hello world!") == "aGVsbG8gd29ybGQh");
}