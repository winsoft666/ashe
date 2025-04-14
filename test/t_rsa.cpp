#include "catch.hpp"
#include "ashe/rsa.h"

using namespace ashe;

TEST_CASE("RSATest") {
    std::string pub, priv;
    RSAGenKeys(pub, priv);

    REQUIRE(pub.length() > 0);
    REQUIRE(priv.length() > 0);

    std::string in = "1234567890qwertyuiopasdfghjklzxcvbnm,./ !@#$%^&*()_+~";

    std::vector<uint8_t> vIn;
    vIn.resize(in.length());
    memcpy(vIn.data(), in.c_str(), in.length());

    std::string cipher;
    REQUIRE(RSAEncrypt(vIn, pub, cipher));
    REQUIRE(cipher.size() > 0);

    std::vector<uint8_t> vOut;
    REQUIRE(RSADecrypt(cipher, priv, vOut));

    REQUIRE(vIn == vOut);
}



TEST_CASE("RSATest-Str") {
    std::string pub, priv;
    RSAGenKeys(pub, priv);

    REQUIRE(pub.length() > 0);
    REQUIRE(priv.length() > 0);

    std::string in = "1234567890qwertyuiopasdfghjklzxcvbnm,./ !@#$%^&*()_+~";

    std::string cipher;
    REQUIRE(RSAEncrypt(in, pub, cipher));
    REQUIRE(cipher.size() > 0);

    std::string out;
    REQUIRE(RSADecrypt(cipher, priv, out));

    REQUIRE(in == out);
}
