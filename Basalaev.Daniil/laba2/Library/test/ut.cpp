#include "catch2/catch_all.hpp"
#include "Common/Alias.hpp"
#include "Common/Http.hpp"
#include "Conn/conn_sock.hpp"
#include "Conn/conn_fifo.hpp"

static constexpr alias::id_t ID = 1;
static constexpr auto MAX_SIZE = 1024;

TEST_CASE("HTTP request")
{
    static std::string const requestMsg = "http://POST/1/Book 1";
    SECTION("parse request [sunny]")
    {
        // parse from string
        auto req = http::request::parse(requestMsg);
        REQUIRE(req);
        CHECK(req->type == http::OperationType_e::POST);
        CHECK(req->id == ID);
        CHECK(req->bookName == "Book 1");
        CHECK(req->toString() == requestMsg);

        // crate
        http::request req2{
            .type = http::OperationType_e::POST,
            .id = ID,
            .bookName = "Book 1"
        };
        CHECK(req2.toString() == requestMsg);
    }
    SECTION("parse request [rainy][invalid protocol prefix]")
    {
        REQUIRE_FALSE(http::request::parse("https://POST/1/Book 1"));
        REQUIRE_FALSE(http::request::parse("HTTP://POST/1/Book 1"));
        REQUIRE_FALSE(http::request::parse("__1111http://POST/1/Book 1"));
    }
    SECTION("parse request [rainy][unsupported operation]")
    {
        REQUIRE_FALSE(http::request::parse("http://GET/1/Book 1"));
        REQUIRE_FALSE(http::request::parse("http://PLEASE/1/Book 1"));
    }
    SECTION("parse request [rainy][request without id]")
    {
        REQUIRE_FALSE(http::request::parse("http://GET/Book 1"));
    }
    SECTION("parse request [rainy][request without bookName]")
    {
        REQUIRE_FALSE(http::request::parse("http://GET/1"));
    }
}

TEST_CASE("HTTP response")
{
    static std::string const responseMsg = "http://head/0/OK";
    SECTION("parse response [sunny]")
    {
        // parse from string
        auto rsp = http::response::parse(responseMsg);
        REQUIRE(rsp);
        CHECK(rsp->status == http::OperationStatus_e::OK);
        CHECK(rsp->id == alias::HOST_ID);

        // crate
        http::response rsp2{
            .id = alias::HOST_ID,
            .status = http::OperationStatus_e::OK
        };
        CHECK(rsp2.toString() == responseMsg);
    }
    SECTION("parse response [rainy][invalid protocol prefix]")
    {
        REQUIRE_FALSE(http::response::parse("https://head/0/OK"));
        REQUIRE_FALSE(http::response::parse("HTTP://head/0/OK"));
        REQUIRE_FALSE(http::response::parse("__1111http://head/0/OK"));
        REQUIRE_FALSE(http::response::parse("http://POST/0/OK"));
        REQUIRE_FALSE(http::response::parse("http://POST/1/Book 1"));
    }
    SECTION("parse response [rainy][unsupported status]")
    {
        REQUIRE_FALSE(http::response::parse("http://head/0/NO_CONTENT"));
        REQUIRE_FALSE(http::response::parse("http://PLEASE/1/404"));
    }
    SECTION("parse response [rainy][response without id]")
    {
        REQUIRE_FALSE(http::response::parse("http://head/OK"));
    }
    SECTION("parse response [rainy][response without stauts]")
    {
        REQUIRE_FALSE(http::response::parse("http://head/1"));
    }
}

TEST_CASE("HTTP notification")
{
    // TODO
    //! format notification: http://notification/{books}
}

TEST_CASE("Socket connection")
{
    // OS need time, that clear socket connection, so that
    // run this test with interav several seconds
    static alias::port_t PORT = 10101;
    SECTION("Sanity connection and communication")
    {
        auto hostSocketConn = ConnSock::crateHostSocket(PORT);
        REQUIRE(hostSocketConn);

        auto clientSocketConn = ConnSock::crateClientSocket(PORT);
        REQUIRE(clientSocketConn);

        auto hostSocketConnAccepted = hostSocketConn->Accept();
        REQUIRE(hostSocketConnAccepted);

        std::string const msg = "LALALA";
        REQUIRE(clientSocketConn->Write(msg.c_str(), msg.size()));

        char buffer[MAX_SIZE] = {0};
        REQUIRE(hostSocketConnAccepted->Read(buffer, MAX_SIZE));
        CHECK(std::string(buffer) == msg);

        REQUIRE(hostSocketConnAccepted->Write(msg.c_str(), msg.size()));

        buffer[MAX_SIZE] = {0};
        REQUIRE(clientSocketConn->Read(buffer, MAX_SIZE));
        CHECK(std::string(buffer) == msg);
    }
    SECTION("Failed to setup connection, because of different host's PORT")
    {
        PORT++; // For new connection new port
        auto hostSocketConn = ConnSock::crateHostSocket(PORT);
        REQUIRE(hostSocketConn);

        auto clientSocketConn = ConnSock::crateClientSocket(PORT + 1);
        REQUIRE_FALSE(clientSocketConn);
    }
}

TEST_CASE("Fifo connection")
{
    static constexpr auto FIFO_PATH = "/tmp/my_fifo";
    SECTION("Sanity connection and communication")
    {
        auto hostFifoConn = ConnFifo::crateHostFifo(FIFO_PATH);
        REQUIRE(hostFifoConn);

        auto clientFifoConn = ConnFifo::crateClientFifo(FIFO_PATH);
        REQUIRE(clientFifoConn);

        std::string const msg = "LALALA";
        REQUIRE(clientFifoConn->Write(msg.c_str(), msg.size()));

        char buffer[MAX_SIZE] = {0};
        REQUIRE(hostFifoConn->Read(buffer, MAX_SIZE));
        CHECK(std::string(buffer) == msg);

        REQUIRE(hostFifoConn->Write(msg.c_str(), msg.size()));

        buffer[MAX_SIZE] = {0};
        REQUIRE(clientFifoConn->Read(buffer, MAX_SIZE));
        CHECK(std::string(buffer) == msg);
    }
    SECTION("Failed to setup connection, because of different fifo's path")
    {
        auto hostFifoConn = ConnFifo::crateHostFifo(FIFO_PATH);
        REQUIRE(hostFifoConn);

        auto clientFifoConn = ConnFifo::crateClientFifo(FIFO_PATH + std::string("Lalala"));
        REQUIRE_FALSE(clientFifoConn);
    }
}
