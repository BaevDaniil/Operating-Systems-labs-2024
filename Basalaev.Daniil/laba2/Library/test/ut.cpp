#include "catch2/catch_all.hpp"
#include "Common/Alias.hpp"
#include "Common/Http.hpp"
#include "Common/Reader.hpp"
#include "Conn/conn_sock.hpp"
#include "Conn/conn_fifo.hpp"
#include "Conn/conn_pipe.hpp"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>

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
    static std::string const responseMsg = "http://notification/[{\"amount\":5,\"name\":\"Book 1\"},{\"amount\":3,\"name\":\"Book 2\"},{\"amount\":0,\"name\":\"Book 3\"}]";
     
    Book book1 = {.name = "Book 1", .amount = 5};
    Book book2 = {.name = "Book 2", .amount = 3};
    Book book3 = {.name = "Book 3", .amount = 0};
    SECTION("parse notification [sunny]")
    {
        // parse from string
        auto notify = http::notification::parse(responseMsg);
        REQUIRE(notify);
        REQUIRE(notify->books.size() == 3);
        CHECK(notify->books[0].name == book1.name);
        CHECK(notify->books[0].amount == book1.amount);
        CHECK(notify->books[1].name == book2.name);
        CHECK(notify->books[1].amount == book2.amount);
        CHECK(notify->books[2].name == book3.name);
        CHECK(notify->books[2].amount == book3.amount);

        // crate
        http::notification notify2{.books = {book1, book2, book3} };
        CHECK(notify2.toString() == responseMsg);
    }
    SECTION("parse notification [rainy][invalid protocol prefix]")
    {
        REQUIRE_FALSE(http::notification::parse("https://notification/[{\"amount\":5,\"name\":\"Book 1\"}]"));
        REQUIRE_FALSE(http::notification::parse("HTTP://notification/[{\"amount\":5,\"name\":\"Book 1\"}]"));
        REQUIRE_FALSE(http::notification::parse("__1111https://notification/[{\"amount\":5,\"name\":\"Book 1\"}]"));
        REQUIRE_FALSE(http::notification::parse("https://notificationLA/[{\"amount\":5,\"name\":\"Book 1\"}]\""));
        REQUIRE_FALSE(http::notification::parse("https://notification/POST/[{\"amount\":5,\"name\":\"Book 1\"}]"));
    }
    SECTION("parse notification [rainy][notification some book without amount]")
    {
        REQUIRE_FALSE(http::notification::parse("http://notification/[{\"name\":\"Book 1\"}]"));
    }
    SECTION("parse notification [rainy][notification some book without name]")
    {
        REQUIRE_FALSE(http::notification::parse("http://notification/[{\"amount\":5}]"));
    }
    SECTION("parse notification [rainy][notification without array of books]")
    {
        REQUIRE_FALSE(http::notification::parse("http://notification/"));
        REQUIRE_FALSE(http::notification::parse("http://notification/\"amount\":5"));
    }
}

TEST_CASE("Reader::parse")
{
    SECTION("parse [sunny]")
    {
        auto const testFilePath = "test_books.json";
        QFile file(testFilePath);
        REQUIRE(file.open(QIODevice::WriteOnly));

        QJsonArray booksArray = {
            QJsonObject{{"name", "Book 1"}, {"amount", 1}},
            QJsonObject{{"name", "Book 2"}, {"amount", 2}},
            QJsonObject{{"name", "Book 3"}, {"amount", 3}}
        };

        QJsonObject rootObj;
        rootObj["books"] = booksArray;

        QJsonDocument doc(rootObj);
        file.write(doc.toJson());
        file.close();

        std::optional<alias::book_container_t> result = Reader::parse(testFilePath);
        REQUIRE(result.has_value());
        REQUIRE(result->size() == 3);

        CHECK((*result)[0].name == "Book 1");
        CHECK((*result)[0].amount == 1);
        CHECK((*result)[1].name == "Book 2");
        CHECK((*result)[1].amount == 2);
        CHECK((*result)[2].name == "Book 3");
        CHECK((*result)[2].amount == 3);

        QFile::remove(testFilePath);
    }
    SECTION("parse [rainy][Invalid File Path]")
    {
        std::optional<alias::book_container_t> result = Reader::parse("invalid_path.json");
        REQUIRE_FALSE(result.has_value());
    }
    SECTION("parse [rainy][Invalid JSON Format]")
    {
        auto const testFilePath = "invalid_json.json";
        QFile file(testFilePath);
        REQUIRE(file.open(QIODevice::WriteOnly));

        file.write("{ invalid json }");
        file.close();

        std::optional<alias::book_container_t> result = Reader::parse(testFilePath);
        REQUIRE_FALSE(result.has_value());

        QFile::remove(testFilePath);
    }
    SECTION("parse [rainy][Missing Books Array]")
    {
        auto const testFilePath = "missing_books.json";
        QFile file(testFilePath);
        REQUIRE(file.open(QIODevice::WriteOnly));

        QJsonObject rootObj; // JSON без массива "books"
        QJsonDocument doc(rootObj);
        file.write(doc.toJson());
        file.close();

        std::optional<alias::book_container_t> result = Reader::parse(testFilePath);
        REQUIRE_FALSE(result.has_value());

        QFile::remove(testFilePath);
    }
    SECTION("parse [rainy][Invalid Book Entry]")
    {
        auto const testFilePath = "invalid_book_entry.json";
        QFile file(testFilePath);
        REQUIRE(file.open(QIODevice::WriteOnly));

        QJsonArray booksArray = {
            QJsonObject{{"name", "Book 1"}, {"amount", 1}},
            QJsonObject{{"invalid_field", "Book 2"}, {"amount", 2}},
            QJsonObject{{"name", "Book 3"}, {"amount", 3}}
        };

        QJsonObject rootObj;
        rootObj["books"] = booksArray;

        QJsonDocument doc(rootObj);
        file.write(doc.toJson());
        file.close();

        std::optional<alias::book_container_t> result = Reader::parse(testFilePath);
        REQUIRE_FALSE(result.has_value());

        QFile::remove(testFilePath);
    }
}

TEST_CASE("Socket connection")
{
    // OS need time, that clear socket connection, so that
    // run this test with interav several seconds
    static alias::port_t PORT = 10101;
    SECTION("Sanity connection and communication")
    {
        auto hostSocketConn = ConnSock::craeteHostSocket(PORT);
        REQUIRE(hostSocketConn);

        auto clientSocketConn = ConnSock::craeteClientSocket(PORT);
        REQUIRE(clientSocketConn);

        auto hostSocketConnAccepted = hostSocketConn->accept();
        REQUIRE(hostSocketConnAccepted);

        std::string const msg = "LALALA";
        REQUIRE(clientSocketConn->write(msg.c_str(), msg.size()));

        char buffer[MAX_SIZE] = {0};
        REQUIRE(hostSocketConnAccepted->read(buffer, MAX_SIZE));
        CHECK(std::string(buffer) == msg);

        REQUIRE(hostSocketConnAccepted->write(msg.c_str(), msg.size()));

        buffer[MAX_SIZE] = {0};
        REQUIRE(clientSocketConn->read(buffer, MAX_SIZE));
        CHECK(std::string(buffer) == msg);
    }
    SECTION("Failed to setup connection, because of different host's PORT")
    {
        PORT++; // For new connection new port
        auto hostSocketConn = ConnSock::craeteHostSocket(PORT);
        REQUIRE(hostSocketConn);

        auto clientSocketConn = ConnSock::craeteClientSocket(PORT + 1);
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
        REQUIRE(clientFifoConn->write(msg.c_str(), msg.size()));

        char buffer[MAX_SIZE] = {0};
        REQUIRE(hostFifoConn->read(buffer, MAX_SIZE));
        CHECK(std::string(buffer) == msg);

        REQUIRE(hostFifoConn->write(msg.c_str(), msg.size()));

        buffer[MAX_SIZE] = {0};
        REQUIRE(clientFifoConn->read(buffer, MAX_SIZE));
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

TEST_CASE("Pipe connection")
{
    SECTION("Sanity connection and communication")
    {
        auto [hostPipeConn, clientPipeConn] = ConnPipe::createPipeConns();
        REQUIRE(hostPipeConn);
        REQUIRE(clientPipeConn);

        std::string const msg = "LALALA";
        REQUIRE(clientPipeConn->write(msg.c_str(), msg.size()));

        char buffer[MAX_SIZE] = {0};
        REQUIRE(hostPipeConn->read(buffer, MAX_SIZE));
        CHECK(std::string(buffer) == msg);

        REQUIRE(hostPipeConn->write(msg.c_str(), msg.size()));

        buffer[MAX_SIZE] = {0};
        REQUIRE(clientPipeConn->read(buffer, MAX_SIZE));
        CHECK(std::string(buffer) == msg);
    }
}
