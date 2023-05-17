#include "catch.hpp"
#include "ashe/win_http.hpp"
#include "ashe/md5.hpp"
#include "ashe/win_http_client.hpp"

TEST_CASE("Http1", "get") {
    ashe::WinHttp http;
    REQUIRE(http.openSession());
    REQUIRE(http.openConnect(L"https://www.baidu.com"));
    REQUIRE(http.openRequest());
    REQUIRE(http.sendRequest());
    REQUIRE(http.receiveResponse());
    REQUIRE(http.getResponseStatusCode() == 200);
    std::vector<unsigned char> rspBody;

    REQUIRE(http.getResponseBody(rspBody));
    REQUIRE(rspBody.size() > 1000);
}

TEST_CASE("Http2", "download") {
    ashe::WinHttp http;
    REQUIRE(http.openSession());
    REQUIRE(http.openConnect(L"https://dlie.sogoucdn.com/se/sogou_explorer_11.0.1.34700_0000.exe"));
    REQUIRE(http.openRequest());
    REQUIRE(http.sendRequest());
    REQUIRE(http.receiveResponse());
    REQUIRE(http.saveResponseBodyToFile(L"D:\\sogou_explorer_11.0.1.34700_0000.exe"));
    REQUIRE(ashe::MD5::GetFileMD5(L"D:\\sogou_explorer_11.0.1.34700_0000.exe") == "73a0e33385b7fd3c2ce6279f35ef0c0b");
}

TEST_CASE("HttpClient1", "get") {
    using namespace ashe;

    std::shared_ptr<HttpReqDatagram> req = std::make_shared<HttpReqDatagram>();
    req->setMethod(HttpReqDatagram::METHOD::GET);
    req->setUrl("https://www.baidu.com");

    WinHttpClient client;
    client.request(req, [](long code, unsigned long usedMill, const HttpRspDatagram& rspDg) {
        REQUIRE(code == 0);
        REQUIRE(rspDg.body().size() > 0);
    });
    client.wait(-1);
}

TEST_CASE("HttpClient2", "get_cancel") {
    using namespace ashe;

    std::shared_ptr<HttpReqDatagram> req = std::make_shared<HttpReqDatagram>();
    req->setMethod(HttpReqDatagram::METHOD::GET);
    req->setUrl("https://www.baidu.com");

    WinHttpClient client;
    client.request(req, [](long code, unsigned long usedMill, const HttpRspDatagram& rspDg) {
        CHECK(code == WinHttpClient::USER_ABORT);
    });
    CHECK(client.wait(10) == false);
    client.abort();
    CHECK(client.wait(-1) == true);
}

TEST_CASE("HttpClient3", "download") {
    using namespace ashe;

    std::shared_ptr<HttpReqDatagram> req = std::make_shared<HttpReqDatagram>();
    req->setMethod(HttpReqDatagram::METHOD::GET);
    req->setUrl("https://dlie.sogoucdn.com/se/sogou_explorer_11.0.1.34700_0000.exe");
    req->setSaveFilePath("D:\\sogou_explorer_11.0.1.34700_0000.exe");

    WinHttpClient client;
    client.request(req, [](long code, unsigned long usedMill, const HttpRspDatagram& rspDg) {
        CHECK(code == 0);
        CHECK(ashe::MD5::GetFileMD5(L"D:\\sogou_explorer_11.0.1.34700_0000.exe") == "73a0e33385b7fd3c2ce6279f35ef0c0b");
    });
    CHECK(client.wait(-1));
}

TEST_CASE("HttpClient4", "download") {
    using namespace ashe;

    std::shared_ptr<HttpReqDatagram> req = std::make_shared<HttpReqDatagram>();
    req->setMethod(HttpReqDatagram::METHOD::GET);
    req->setUrl("https://dlie.sogoucdn.com/se/sogou_explorer_11.0.1.34700_0000.exe");
    req->setSaveFilePath("D:\\sogou_explorer_11.0.1.34700_0000.exe");

    WinHttpClient client;
    client.request(req, [](long code, unsigned long usedMill, const HttpRspDatagram& rspDg) {
        CHECK(code == (long)WinHttpClient::USER_ABORT);
    });

    CHECK(client.wait(100) == false);
    client.abort();
    CHECK(client.wait(-1) == true);
}