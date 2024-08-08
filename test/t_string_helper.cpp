#include "catch.hpp"
#include "ashe/string_helper.h"

using namespace ashe;

// Test: string operate.
//
TEST_CASE("StringHelperTest1") {
    REQUIRE(ToLower('c') == 'c');
    REQUIRE(ToLower('C') == 'c');
    REQUIRE(ToUpper('A') == 'A');
    REQUIRE(ToUpper('a') == 'A');
    REQUIRE(ToLower(L'c') == L'c');
    REQUIRE(ToLower(L'C') == L'c');
    REQUIRE(ToUpper(L'A') == L'A');
    REQUIRE(ToUpper(L'a') == L'A');
    REQUIRE(ToLower("1234567890abcdefABCDEF#@!%%") == "1234567890abcdefabcdef#@!%%");
    REQUIRE(ToLower(L"1234567890abcdefABCDEF#@!%%") == L"1234567890abcdefabcdef#@!%%");
    REQUIRE(ToUpper("1234567890abcdefABCDEF#@!%%") == "1234567890ABCDEFABCDEF#@!%%");
    REQUIRE(ToUpper(L"1234567890abcdefABCDEF#@!%%") == L"1234567890ABCDEFABCDEF#@!%%");

    REQUIRE(IsDigit("3.14") == false);
    REQUIRE(IsDigit("3a12") == false);
    REQUIRE(IsDigit("134") == true);

    REQUIRE(IsDigit(L"3.14") == false);
    REQUIRE(IsDigit(L"3a12") == false);
    REQUIRE(IsDigit(L"134") == true);

    REQUIRE(StrTrim("\r\f erase\n\t white-spaces   \n") == "erase\n\t white-spaces");
    REQUIRE(StrTrim(L"\r\f erase\n\t white-spaces   \n") == L"erase\n\t white-spaces");
    REQUIRE(StrTrimLeft(L"\r\f erase\n\t white-spaces   \n") == L"erase\n\t white-spaces   \n");
    REQUIRE(StrTrimLeft("\r\f erase\n\t white-spaces   \n") == "erase\n\t white-spaces   \n");
    REQUIRE(StrTrimRight("\r\f erase\n\t white-spaces   \n") == "\r\f erase\n\t white-spaces");
    REQUIRE(StrTrimRight("\r\f erase\n\t white-spaces   \n") == "\r\f erase\n\t white-spaces");
    REQUIRE(StrTrimRight("\r\f erase\n\t white-spaces   \n", "") == "\r\f erase\n\t white-spaces   \n");
    REQUIRE(StrTrim("\r\f erase\n\t white-spaces   \n", "") == "\r\f erase\n\t white-spaces   \n");
    REQUIRE(StrTrim("\r\f erase\n\t white-spaces   \n", "\r\f") == " erase\n\t white-spaces   \n");

    REQUIRE(IsEqual("abcdefgxyz123#~/", "abcdefgxyz123#~/", false));
    REQUIRE(!IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", false));
    REQUIRE(IsEqual("abcdefgxyz123#~/", "abcdefgxyZ123#~/", true));

    REQUIRE(IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyz123#~/", false));
    REQUIRE(!IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", false));
    REQUIRE(IsEqual(L"abcdefgxyz123#~/", L"abcdefgxyZ123#~/", true));

    REQUIRE(IsStartsWith("1234567890abcdef#@!%%", "1234567890"));
    REQUIRE(IsStartsWith(L"1234567890abcdef#@!%%", L"1234567890"));
    REQUIRE(!IsStartsWith("1234567890abcdef#@!%%", "abcdefg"));
    REQUIRE(!IsStartsWith(L"1234567890abcdef#@!%%", L"abcdefg"));

    REQUIRE(StrContainTimes("123456712", "12") == 2);
    REQUIRE(StrContainTimes(L"123456712", L"12") == 2);
    REQUIRE(StrContainTimes("1234567121", "121") == 1);
    REQUIRE(StrContainTimes(L"1234567121", L"121") == 1);
    REQUIRE(StrContainTimes("123 4567 121", " ") == 2);
    REQUIRE(StrContainTimes(L"123 4567 121", L" ") == 2);

    const std::string s1 = StrFormat("%s's age is %d", "jack", 18);
    REQUIRE(s1 == "jack's age is 18");

    const std::wstring ws1 = StrFormat(L"%ls's age is %d", L"jack", 18);
    REQUIRE(ws1 == L"jack's age is 18");

    std::string bigStrA(2048, 'a');
    bigStrA += "[end]";
    const std::string s2 = StrFormat("%s length is %d", bigStrA.c_str(), bigStrA.length());
    REQUIRE(s2 == bigStrA + " length is " + std::to_string(bigStrA.length()));

    std::wstring bigStrW(2048, L'a');
    bigStrW += L"[end]";
    const std::wstring ws2 = StrFormat(L"%ls length is %d", bigStrW.c_str(), bigStrW.length());
    REQUIRE(ws2 == bigStrW + L" length is " + std::to_wstring(bigStrW.length()));

    std::string bigStrA2(1024, 'c');
    const std::string s3 = StrFormat("%s", bigStrA2.c_str());
    REQUIRE(s3 == bigStrA2);

    std::wstring bigStrW2(1024, L'c');
    const std::wstring sw3 = StrFormat(L"%ls", bigStrW2.c_str());
    REQUIRE(sw3 == bigStrW2);
}

TEST_CASE("StringHelperTest2", "Find") {
    std::wstring s = L"@%SystemRoot%\\system32\\shell32.dll,-10113";
    std::wstring::size_type pos = StrFind(s, L"%systemroot%", 0, true);
    REQUIRE(pos == 1);

    std::wstring::size_type pos2 = StrFind(s, L"%systemroot%", 0, false);
    REQUIRE(pos2 == std::wstring::npos);

    std::wstring::size_type pos3 = StrFind(s, L"%SystemRoot%", 2, true);
    REQUIRE(pos3 == std::wstring::npos);

    std::wstring::size_type pos4 = StrFind(s, L"%SystemRoot%", 1, false);
    REQUIRE(pos4 == 1);

    std::wstring::size_type pos5 = StrFind(s, L"32", 23, false);
    REQUIRE(pos5 == 28);

    std::wstring::size_type pos6 = StrFind(s, L"32", 230, false);
    REQUIRE(pos6 == std::wstring::npos);
}

TEST_CASE("StringHelperTest3", "Repace") {
    std::wstring s = L"@%SystemRoot%\\system32\\%systemroot%.dll,-10113";
    std::wstring s1 = StrReplace(s, L"%systemroot%", L"c:", 0, false);
    REQUIRE(s1 == L"@%SystemRoot%\\system32\\c:.dll,-10113");

    std::wstring s2 = StrReplace(s, L"%systemroot%", L"c:", 0, true);
    REQUIRE(s2 == L"@c:\\system32\\c:.dll,-10113");

    std::wstring s3 = StrReplace(s, L"%systemroot%", L"c:", 200, true);
    REQUIRE(s3 == L"@%SystemRoot%\\system32\\%systemroot%.dll,-10113");
}
