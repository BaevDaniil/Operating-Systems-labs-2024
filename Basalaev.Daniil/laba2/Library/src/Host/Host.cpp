#include "Host.hpp"
#include "HostWindow.hpp"
#include "Common/Http.hpp"
#include "Common/Logger.hpp"

#include <QApplication>
#include <signal.h>
#include <thread>

namespace details
{

auto findBook(alias::book_container_t& books, std::string const& bookName)
{
    for (auto bookIt = books.begin(); bookIt != books.end(); ++bookIt)
    {
        if (bookIt->name == bookName)
        {
            return bookIt;
        }
    }

    return books.end();
}

} // namespace details

static int argc = 0;

Host::Host(SemaphoreLocal& semaphore, connImpl& connection, alias::book_container_t const& books, QObject* parent)
    : NetWorkElementImpl(alias::HOST_ID, semaphore, connection, parent)
    , m_books(books)
{}

Host::~Host() = default;

int Host::start()
{
    LOG_INFO(HOST_LOG, "successfully start");

    std::thread listener(&Host::listen, this); // start listen messages from clients

    QApplication app(argc, nullptr);
    m_window = std::unique_ptr<HostWindow>(new HostWindow("HOST WINDOW", m_books));
    m_window->show();
    int res = app.exec();

    m_isRunning = false; // TODO: make stop
    if (listener.joinable()) { listener.join(); }

    return res;
}

void Host::listen()
{
    while (m_isRunning)
    {
        m_semaphore.wait();

        char buffer[1024] = {0};
        if (m_connection.Read(buffer, sizeof(buffer)))
        {
            if (auto req = http::request::parse(std::string(buffer)))
            {
                LOG_INFO(HOST_LOG, "successfully read msg from client[ID=" + std::to_string(req->id) + "]: " + req->toString());

                if (req->type == http::OperationType_e::POST)
                {
                    handleBookSelected(req->bookName, req->id);
                }
                else
                {
                    handleBookReturned(req->bookName, req->id);
                }
            }
            else
            {
                LOG_ERROR(HOST_LOG, "read, but failed to parse msg from client");
            }
        }

        m_semaphore.post(); // End critical section
        sleep(0.01);
    }
}

void Host::updateClientInfo(utils::ClientInfo clientInfo)
{
    auto it = std::find_if(m_clients.begin(), m_clients.end(), [id = clientInfo.clientId](auto const& client){ return client.clientId == id; });
    if (it == m_clients.end())
    {
        m_clients.emplace_back(std::move(clientInfo));
    }
    else
    {
        *it = std::move(clientInfo);
    }
}

void Host::handleBookSelected(std::string const& bookName, alias::id_t clientId)
{
    http::response rsp{.id = clientId};

    if (auto book = details::findBook(m_books, bookName); book != m_books.end())
    {
        if (book->amount > 0)
        {
            --book->amount;
            rsp.status = http::OperationStatus_e::OK;
        }
        else
        {
            rsp.status = http::OperationStatus_e::FAIL;
        }
    }
    else
    {
        rsp.status = http::OperationStatus_e::FAIL;
    }

    std::string const rspStr = rsp.toString();
    if (m_connection.Write(rspStr.c_str(), rspStr.size()))
    {
        LOG_INFO(HOST_LOG, "write to client: " + rspStr);
        if (rsp.status == http::OperationStatus_e::OK)
        {
            m_window->onSuccessTakeBook(bookName, clientId);
            m_window->updateBooks(m_books);
            updateClientInfo({.clientId = clientId, .readingBook = bookName, .secondsToKill = 100});
            m_window->updateClientsInfo(m_clients);
        }
        else
        {
            m_window->onFailedTakeBook(bookName, clientId);
        }
    }
    else
    {
        LOG_ERROR(HOST_LOG, "failed to write to client: " + rspStr);
        m_window->onFailedTakeBook(bookName, clientId);
    }
}

void Host::handleBookReturned(std::string const& bookName, alias::id_t clientId)
{
    http::response rsp{.id = clientId};

    if (auto book = details::findBook(m_books, bookName); book != m_books.end())
    {
        ++book->amount;
        rsp.status = http::OperationStatus_e::OK;
    }
    else
    {
        rsp.status = http::OperationStatus_e::FAIL;
    }

    std::string const rspStr = rsp.toString();
    if (m_connection.Write(rspStr.c_str(), rspStr.size()))
    {
        LOG_INFO(HOST_LOG, "write to client: " + rspStr);
        if (rsp.status == http::OperationStatus_e::OK)
        {
            m_window->onSuccessReturnBook(bookName, clientId);
            m_window->updateBooks(m_books);
            updateClientInfo({.clientId = clientId, .readingBook = "", .secondsToKill = 100});
            m_window->updateClientsInfo(m_clients);
        }
        else
        {
            m_window->onFailedReturnBook(bookName, clientId);
        }
    }
    else
    {
        LOG_ERROR(HOST_LOG, "failed to write to client: " + rspStr);
        m_window->onFailedReturnBook(bookName, clientId);
    }
}
