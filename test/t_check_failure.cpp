#include "catch.hpp"
#include "ashe/check_failure.h"

using namespace ashe;

#if 0
TEST_CASE("CheckFailure", "") {
    REQUIRE(ASHE_CHECK_FAILURE(false, "test") == true);
    REQUIRE(ASHE_CHECK_FAILURE(true, "test") == false);
    ASHE_UNEXPECTED_EXCEPTION(std::exception("leak"), "test exception");
    REQUIRE(ASHE_CHECK_FAILURE_HRESULT(E_ACCESSDENIED, "test hresult failed") == true);
    REQUIRE(ASHE_CHECK_FAILURE_HRESULT(S_OK, "test hresult failed") == false);
    REQUIRE(ASHE_CHECK_FAILURE_LSTATUS(ERROR_FILE_NOT_FOUND, "test lstatus failed") == true);
    REQUIRE(ASHE_CHECK_FAILURE_LSTATUS(ERROR_SUCCESS, "test lstatus ok") == false);
}
#endif