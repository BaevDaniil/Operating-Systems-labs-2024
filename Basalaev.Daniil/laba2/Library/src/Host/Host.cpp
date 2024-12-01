#include "Host.hpp"
#include "HostWindow.hpp"
#include "Common/Http.hpp"

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
    , m_app(argc, nullptr)
    , m_window("HOST WINDOW", books)
    , m_books(books)
{
    QObject::connect(&m_window, &HostWindow::bookSelected, this, &Host::handleBookSelected);
    QObject::connect(&m_window, &HostWindow::bookReturned, this, &Host::handleBookReturned);
}

Host::~Host() = default;

int Host::start()
{
    std::thread listener(&Host::listen, this); // start listen messages from clients

    m_window.show();
    int res = m_app.exec();

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
                if (req->type == http::OperationType_e::POST)
                {
                    handleBookSelected(req->bookName);
                }
                else
                {
                    handleBookReturned(req->bookName);
                }
            }
        }

        m_semaphore.post(); // End critical section
        sleep(0.01);
    }
}

void Host::handleBookSelected(const std::string& bookName)
{
    http::response rsp{.id = alias::HOST_ID};

    if (auto book = details::findBook(m_books, bookName); book != m_books.end())
    {
        if (book->count > 0)
        {
            --book->count;
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
        // TODO: log
    }
    else
    {
        // TODO: log
    }

    // TODO: update window
}

void Host::handleBookReturned(const std::string& bookName)
{
    http::response rsp{.id = alias::HOST_ID};

    if (auto book = details::findBook(m_books, bookName); book != m_books.end())
    {
        ++book->count;
        rsp.status = http::OperationStatus_e::OK;
    }
    else
    {
        rsp.status = http::OperationStatus_e::FAIL;
    }

    std::string const rspStr = rsp.toString();
    if (m_connection.Write(rspStr.c_str(), rspStr.size()))
    {
        // TODO: log
    }
    else
    {
        // TODO: log
    }

    // TODO: update window
}
