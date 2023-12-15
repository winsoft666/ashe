#include "catch.hpp"
#include "ashe/all.hpp"

// Test: string hash.
//
TEST_CASE("HashTest1", "[stirng hash]") {
    const std::string str = "hello world!!!";
    REQUIRE(ashe::CRC32::GetDataCRC32((const unsigned char*)str.c_str(), str.length()) == "2416a280");
    REQUIRE(ashe::MD5::GetDataMD5((const unsigned char*)str.c_str(), str.length()) == "f6835168c4823ac89c1bc97154a675a8");
    REQUIRE(ashe::SHA1::GetDataSHA1((const unsigned char*)str.c_str(), str.length()) == "8233f28c479ff758b3b4ba9ad66069db68811e59");
    REQUIRE(ashe::SHA256::GetDataSHA256((const unsigned char*)str.c_str(), str.length()) == "a5f4396b45548597f81681147f53c66065d5137f2fbd85e6758a8983107228e4");
    REQUIRE(ashe::SHA512::GetDataSHA512((const unsigned char*)str.c_str(), str.length()) == "dfa6b727753e96a9dffb10947f99b8457ef51a634ea8baa0db0f22712f0bb19ac6719d4446109dc19dfc9629a3b225c0aeeebd61175464a95e35c437cd979a64");
}

// Test: file hash.
//
TEST_CASE("HashTest2", "[file hash]") {
    const std::string str1K(1024, 'a');
    ashe::File file1("hash_test_file.dat");
    REQUIRE(file1.open("wb"));
    REQUIRE(file1.writeFrom((void*)str1K.c_str(), str1K.size(), 0) == str1K.size());
    REQUIRE(file1.close());
#ifdef ASHE_WIN
    REQUIRE(ashe::CRC32::GetFileCRC32(file1.path()) == "7c5597b9");
    REQUIRE(ashe::MD5::GetFileMD5(file1.path()) == "c9a34cfc85d982698c6ac89f76071abd");
    REQUIRE(ashe::SHA1::GetFileSHA1(file1.path()) == "8eca554631df9ead14510e1a70ae48c70f9b9384");
    REQUIRE(ashe::SHA256::GetFileSHA256(file1.path()) == "2edc986847e209b4016e141a6dc8716d3207350f416969382d431539bf292e4a");
    REQUIRE(ashe::SHA512::GetFileSHA512(file1.path()) == "74b22492e3b9a86a9c93c23a69f821ebafa429302c1f4054b4bc37356a4bae056d9ccbc6f24093a25704faaa72bd21a5f337ca9ec92f32369d24e6b9fae954d8");
#else
    REQUIRE(ashe::CRC32::GetFileCRC32(file1.path().string()) == "7c5597b9");
    REQUIRE(ashe::MD5::GetFileMD5(file1.path().string()) == "c9a34cfc85d982698c6ac89f76071abd");
    REQUIRE(ashe::SHA1::GetFileSHA1(file1.path().string()) == "8eca554631df9ead14510e1a70ae48c70f9b9384");
    REQUIRE(ashe::SHA256::GetFileSHA256(file1.path().string()) == "2edc986847e209b4016e141a6dc8716d3207350f416969382d431539bf292e4a");
    REQUIRE(ashe::SHA512::GetFileSHA512(file1.path().string()) == "74b22492e3b9a86a9c93c23a69f821ebafa429302c1f4054b4bc37356a4bae056d9ccbc6f24093a25704faaa72bd21a5f337ca9ec92f32369d24e6b9fae954d8");
#endif
}
