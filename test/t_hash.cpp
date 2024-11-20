#include "catch.hpp"
#include "ashe/sha1.h"
#include "ashe/sha256.h"
#include "ashe/sha512.h"
#include "ashe/crc32.h"
#include "ashe/md5.h"

using namespace ashe;

// Test: string hash.
//
TEST_CASE("HashTest1", "[stirng hash]") {
    const std::string str = "hello world!!!";
    REQUIRE(GetDataCRC32((const unsigned char*)str.c_str(), str.length()) == "2416a280");
    REQUIRE(GetDataMD5((const unsigned char*)str.c_str(), str.length()) == "f6835168c4823ac89c1bc97154a675a8");
    REQUIRE(GetDataSHA1((const unsigned char*)str.c_str(), str.length()) == "8233f28c479ff758b3b4ba9ad66069db68811e59");
    REQUIRE(GetDataSHA256((const unsigned char*)str.c_str(), str.length()) == "a5f4396b45548597f81681147f53c66065d5137f2fbd85e6758a8983107228e4");
    REQUIRE(GetDataSHA512((const unsigned char*)str.c_str(), str.length()) == "dfa6b727753e96a9dffb10947f99b8457ef51a634ea8baa0db0f22712f0bb19ac6719d4446109dc19dfc9629a3b225c0aeeebd61175464a95e35c437cd979a64");
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
    REQUIRE(GetFileCRC32(file1.pathW()) == "7c5597b9");
    REQUIRE(GetFileMD5(file1.pathW()) == "c9a34cfc85d982698c6ac89f76071abd");
    REQUIRE(GetFileSHA1(file1.pathW()) == "8eca554631df9ead14510e1a70ae48c70f9b9384");
    REQUIRE(GetFileSHA256(file1.pathW()) == "2edc986847e209b4016e141a6dc8716d3207350f416969382d431539bf292e4a");
    REQUIRE(GetFileSHA512(file1.pathW()) == "74b22492e3b9a86a9c93c23a69f821ebafa429302c1f4054b4bc37356a4bae056d9ccbc6f24093a25704faaa72bd21a5f337ca9ec92f32369d24e6b9fae954d8");
#else
    REQUIRE(GetFileCRC32(file1.pathW()) == "7c5597b9");
    REQUIRE(GetFileMD5(file1.pathW()) == "c9a34cfc85d982698c6ac89f76071abd");
    REQUIRE(GetFileSHA1(file1.pathW()) == "8eca554631df9ead14510e1a70ae48c70f9b9384");
    REQUIRE(GetFileSHA256(file1.pathW()) == "2edc986847e209b4016e141a6dc8716d3207350f416969382d431539bf292e4a");
    REQUIRE(GetFileSHA512(file1.pathW()) == "74b22492e3b9a86a9c93c23a69f821ebafa429302c1f4054b4bc37356a4bae056d9ccbc6f24093a25704faaa72bd21a5f337ca9ec92f32369d24e6b9fae954d8");
#endif
}
