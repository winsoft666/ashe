#include "catch.hpp"
#include "ashe/win_signature.h"

#if 0
#ifdef ASHE_WIN
TEST_CASE("WinSignature") {
    using namespace ashe;
    WinSignature sign(L"D:\\MicrosoftEdgeWebview2Setup.exe");

    WinSignature::SignVerifyResult ret = sign.verify();

    std::wstring signer = sign.getSigner();
}
#endif
#endif