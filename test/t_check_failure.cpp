#include "catch.hpp"
#include "ashe/check_failure.h"

using namespace ashe;

TEST_CASE("CheckFailure", "") {
    REQUIRE(ASHE_CHECK_FAILURE(false, L"test") == true);
    REQUIRE(ASHE_CHECK_FAILURE(true, L"test") == false);
    ASHE_UNEXPECTED_EXCEPTION(std::exception("leak"), L"test exception");
    REQUIRE(ASHE_CHECK_FAILURE_HRESULT(E_ACCESSDENIED, L"test hresult failed") == true);
    REQUIRE(ASHE_CHECK_FAILURE_HRESULT(S_OK, L"test hresult failed") == false);
    REQUIRE(ASHE_CHECK_FAILURE_LSTATUS(ERROR_FILE_NOT_FOUND, L"test lstatus failed") == true);
    REQUIRE(ASHE_CHECK_FAILURE_LSTATUS(ERROR_SUCCESS, L"test lstatus ok") == false);
}