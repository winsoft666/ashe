#include "catch.hpp"
#include "ashe/http.h"
#include "ashe/md5.h"
#include "ashe/http_client.h"

TEST_CASE("Http1", "get") {
    ashe::Http http;
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
    ashe::Http http;
    REQUIRE(http.openSession());
    REQUIRE(http.openConnect(L"https://dlie.sogoucdn.com/se/sogou_explorer_11.0.1.34700_0000.exe"));
    REQUIRE(http.openRequest());
    REQUIRE(http.sendRequest());
    REQUIRE(http.receiveResponse());
    REQUIRE(http.saveResponseBodyToFile(L".\\sogou_explorer_11.0.1.34700_0000.exe"));
    REQUIRE(ashe::MD5::GetFileMD5(L".\\sogou_explorer_11.0.1.34700_0000.exe") == "73a0e33385b7fd3c2ce6279f35ef0c0b");
}

TEST_CASE("HttpClient1", "get") {
    using namespace ashe;

    std::shared_ptr<HttpReqDatagram> req = std::make_shared<HttpReqDatagram>();
    req->setMethod(HttpReqDatagram::METHOD::GET);
    req->setUrl("https://www.baidu.com");

    HttpClient client;
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

    HttpClient client;
    client.request(req, [](long code, unsigned long usedMill, const HttpRspDatagram& rspDg) {
        CHECK(code == HttpClient::USER_ABORT);
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
    req->setSaveFilePath(".\\sogou_explorer_11.0.1.34700_0000.exe");

    HttpClient client;
    client.request(req, [](long code, unsigned long usedMill, const HttpRspDatagram& rspDg) {
        CHECK(code == 0);
        CHECK(ashe::MD5::GetFileMD5(L".\\sogou_explorer_11.0.1.34700_0000.exe") == "73a0e33385b7fd3c2ce6279f35ef0c0b");
    });
    CHECK(client.wait(-1));
}

TEST_CASE("HttpClient4", "download") {
    using namespace ashe;

    std::shared_ptr<HttpReqDatagram> req = std::make_shared<HttpReqDatagram>();
    req->setMethod(HttpReqDatagram::METHOD::GET);
    req->setUrl("https://dlie.sogoucdn.com/se/sogou_explorer_11.0.1.34700_0000.exe");
    req->setSaveFilePath(".\\sogou_explorer_11.0.1.34700_0000.exe");

    HttpClient client;
    client.request(req, [](long code, unsigned long usedMill, const HttpRspDatagram& rspDg) {
        CHECK(code == (long)HttpClient::USER_ABORT);
    });

    CHECK(client.wait(100) == false);
    client.abort();
    CHECK(client.wait(-1) == true);
}