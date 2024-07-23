#include "catch.hpp"
#include "ashe/string_helper.h"

// Test: string operate.
//
TEST_CASE("StringHelperTest1") {
    REQUIRE(ashe::ToLower('c') == 'c');
    REQUIRE(ashe::ToLower('C') == 'c');
    REQUIRE(ashe::ToUpper('A') == 'A');
    REQUIRE(ashe::ToUpper('a') == 'A');
    REQUIRE(ashe::ToLower(L'c') == L'c');
    REQUIRE(ashe::ToLower(L'C') == L'c');
    REQUIRE(ashe::ToUpper(L'A') == L'A');
    REQUIRE(ashe::ToUpper(L'a') == L'A');
    REQUIRE(ashe::ToLower("1234567890abcdefABCDEF#@!%%") == "1234567890abcdefabcdef#@!%%");
    REQUIRE(ashe::ToLower(L"1234567890abcdefABCDEF#@!%%") == L"1234567890abcdefabcdef#@!%%");
    REQUIRE(ashe::ToUpper("1234567890abcdefABCDEF#@!%%") == "1234567890ABCDEFABCDEF#@!%%");
    REQUIRE(ashe::ToUpper(L"1234567890abcdefABCDEF#@!%%") == L"1234567890ABCDEFABCDEF#@!%%");

    REQUIRE(ashe::IsDigit("3.14") == false);
    REQUIRE(ashe::IsDigit("3a12") == false);
    REQUIRE(ashe::IsDigit("134") == true);

    REQUIRE(ashe::IsDigit(L"3.14") == false);
    REQUIRE(ashe::IsDigit(L"3a12") == false);
    REQUIRE(ashe::IsDigit(L"134") == true);

    REQUIRE(ashe::Trim("\r\f erase\n\t white-spaces   \n") == "erase\n\t white-spaces");
    REQUIRE(ashe::Trim(L"\r\f erase\n\t white-spaces   \n") == L"erase\n\t white-spaces");
    REQUIRE(ashe::TrimLeft(L"\r\f erase\n\t white-spaces   \n") == L"erase\n\t white-spaces   \n");
    REQUIRE(ashe::TrimLeft("\r\f erase\n\t white-spaces   \n") == "erase\n\t white-spaces   \n");
    REQUIRE(ashe::TrimRight("\r\f erase\n\t white-spaces   \n") == "\r\f erase\n\t white-spaces");
    REQUIRE(ashe::TrimRight("\r\f erase\n\t white-spaces   \n") == "\r\f erase\n\t white-spaces");
    REQUIRE(ashe::TrimRight("\r\f erase\n\t white-spaces   \n", "") == "\r\f erase\n\t white-spaces   \n");
    REQUIRE(ashe::Trim("\r\f erase\n\t white-spaces   \n", "") == "\r\f erase\n\t white-spaces   \n");
    REQUIRE(ashe::Trim("\r\f erase\n\t white-spaces   \n", "\r\f") == " erase\n\t white-spaces   \n");

    REQUIRE(ashe::IsEqual("abcdefgxyz123#~/", "abcdefgxyz123#~/", false));
    REQUIRE(!ashe::IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", false));
    REQUIRE(ashe::IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", true));

    REQUIRE(ashe::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyz123#~/", false));
    REQUIRE(!ashe::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", false));
    REQUIRE(ashe::IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", true));

    REQUIRE(ashe::IsStartsWith("1234567890abcdef#@!%%", "1234567890"));
    REQUIRE(ashe::IsStartsWith(L"1234567890abcdef#@!%%", L"1234567890"));
    REQUIRE(!ashe::IsStartsWith("1234567890abcdef#@!%%", "abcdefg"));
    REQUIRE(!ashe::IsStartsWith(L"1234567890abcdef#@!%%", L"abcdefg"));

    REQUIRE(ashe::ContainTimes("123456712", "12") == 2);
    REQUIRE(ashe::ContainTimes(L"123456712", L"12") == 2);
    REQUIRE(ashe::ContainTimes("1234567121", "121") == 1);
    REQUIRE(ashe::ContainTimes(L"1234567121", L"121") == 1);
    REQUIRE(ashe::ContainTimes("123 4567 121", " ") == 2);
    REQUIRE(ashe::ContainTimes(L"123 4567 121", L" ") == 2);

    const std::string s1 = ashe::StringPrintf("%s's age is %d", "jack", 18);
    REQUIRE(s1 == "jack's age is 18");

    const std::wstring ws1 = ashe::StringPrintf(L"%ls's age is %d", L"jack", 18);
    REQUIRE(ws1 == L"jack's age is 18");

    std::string bigStrA(2048, 'a');
    bigStrA += "[end]";
    const std::string s2 = ashe::StringPrintf("%s length is %d", bigStrA.c_str(), bigStrA.length());
    REQUIRE(s2 == bigStrA + " length is " + std::to_string(bigStrA.length()));

    std::wstring bigStrW(2048, L'a');
    bigStrW += L"[end]";
    const std::wstring ws2 = ashe::StringPrintf(L"%ls length is %d", bigStrW.c_str(), bigStrW.length());
    REQUIRE(ws2 == bigStrW + L" length is " + std::to_wstring(bigStrW.length()));

    std::string bigStrA2(1024, 'c');
    const std::string s3 = ashe::StringPrintf("%s", bigStrA2.c_str());
    REQUIRE(s3 == bigStrA2);

    std::wstring bigStrW2(1024, L'c');
    const std::wstring sw3 = ashe::StringPrintf(L"%ls", bigStrW2.c_str());
    REQUIRE(sw3 == bigStrW2);
}

TEST_CASE("StringHelperTest2", "Find") {
    std::wstring s = L"@%SystemRoot%\\system32\\shell32.dll,-10113";
    std::wstring::size_type pos = ashe::Find(s, L"%systemroot%", 0, true);
    REQUIRE(pos == 1);

    std::wstring::size_type pos2 = ashe::Find(s, L"%systemroot%", 0, false);
    REQUIRE(pos2 == std::wstring::npos);

    std::wstring::size_type pos3 = ashe::Find(s, L"%SystemRoot%", 2, true);
    REQUIRE(pos3 == std::wstring::npos);

    std::wstring::size_type pos4 = ashe::Find(s, L"%SystemRoot%", 1, false);
    REQUIRE(pos4 == 1);

    std::wstring::size_type pos5 = ashe::Find(s, L"32", 23, false);
    REQUIRE(pos5 == 28);

    std::wstring::size_type pos6 = ashe::Find(s, L"32", 230, false);
    REQUIRE(pos6 == std::wstring::npos);
}

TEST_CASE("StringHelperTest3", "Repace") {
    std::wstring s = L"@%SystemRoot%\\system32\\%systemroot%.dll,-10113";
    std::wstring s1 = ashe::Replace(s, L"%systemroot%", L"c:", 0, false);
    REQUIRE(s1 == L"@%SystemRoot%\\system32\\c:.dll,-10113");

    std::wstring s2 = ashe::Replace(s, L"%systemroot%", L"c:", 0, true);
    REQUIRE(s2 == L"@c:\\system32\\c:.dll,-10113");

    std::wstring s3 = ashe::Replace(s, L"%systemroot%", L"c:", 200, true);
    REQUIRE(s3 == L"@%SystemRoot%\\system32\\%systemroot%.dll,-10113");
}
