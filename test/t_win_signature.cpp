#include "catch.hpp"
#include "ashe/arch.h"
#ifdef ASHE_WIN
#include "ashe/win/signature.h"

using namespace ashe::win;

#if 1
TEST_CASE("Signature") {
    Signature sign(L"D:\\GameMod_XXX_2.0.2.1.exe");

    Signature::SignVerifyResult ret = sign.verify();

    std::wstring signer = sign.getSigner();
}
#endif
#endif