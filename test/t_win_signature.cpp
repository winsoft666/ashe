#include "catch.hpp"
#include "ashe/arch.h"
#ifdef ASHE_WIN
#include "ashe/win/signature.h"

using namespace ashe::win;

#if 0
TEST_CASE("Signature") {
    Signature sign(L"D:\\MicrosoftEdgeWebview2Setup.exe");

    Signature::SignVerifyResult ret = sign.verify();

    std::wstring signer = sign.getSigner();
}
#endif
#endif