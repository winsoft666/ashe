#include "catch.hpp"
#include "ashe/variant.hpp"
#include <string>

TEST_CASE("Variant1", "") {
    ashe::variant<int, float, std::string, std::wstring, bool> var;
    CHECK(var.index() == 0);
    CHECK(ashe::holds_alternative<int>(var));
    CHECK(ashe::get<int>(var) == 0);

    var = "test";
    CHECK(var.index() == 2);
    CHECK(ashe::holds_alternative<std::string>(var));
    CHECK(ashe::get<std::string>(var) == "test");

    var = L"test";
    CHECK(var.index() == 3);
    CHECK(ashe::holds_alternative<std::wstring>(var));
    CHECK(ashe::get<std::wstring>(var) == L"test");

    var = true;
    CHECK(var.index() == 4);
    CHECK(ashe::holds_alternative<bool>(var));
    CHECK(ashe::get<bool>(var) == true);

    if (ashe::holds_alternative<int>(var))
        int i = ashe::get<int>(var);
}