#include "Client.hpp"
#include "ClientWindow.hpp"
#include "Common/Http.hpp"

#include <QApplication>
#include <thread>

static int argc = 0;

Client::Client(alias::id_t id, Semaphore& semaphore, conn& connection, alias::book_container_t const& books, QObject* parent)
    : NetWorkElementImpl(id, semaphore, connection, parent)
    , m_app(argc, nullptr)
    , m_window(books)
{
    QObject::connect(&m_window, &ClientWindow::bookSelected, this, handleBookSelected);
    QObject::connect(&m_window, &ClientWindow::bookReturned, this, handleBookReturned);
}

Client::~Client() = default;

int Client::start()
{
    std::thread listener(listen(), this) // start listen messages from host

    m_window.show();
    int res = m_app.exec();

    m_isRanning = false; // TODO: make stop
    if (listener.joinable()) { listener.join(); }

    return res;
}

void Client::listen()
{
    while (m_isRunning)
    {
        m_semaphore.wait();

        char buffer[1024] = {0};
        if (m_connection.read(buffer, sizeof(buffer)))
        {
            // TODO: check for notifications
            if (auto rsp = http::response::parse(std::string(buffer)))
            {
                if (rsp->id != getId())
                {
                    // TODO: log
                }
                else if (rsp->status == http::OperationStatus_e::OK)
                {
                    m_window.onSuccessPostBook();
                }
                else
                {
                    m_window.onFailedTakeBook();
                }
            }
            else
            {
                // TODO: log
            }
        }

        m_semaphore.Post(); // End critical section
        sleep(0.01);
    }
}

Client::handleBookSelected(const std::string& bookName)
{
    std::string const reqStr = http::request{.type = http::OperationType_e::POST, .id = getId(), .bookName = bookName}.toString();
    if (conn.Write(reqStr.c_str(), reqStr.size()))
    {
        // TODO: log
    }
    else
    {
        // TODO: log
    }
}

Client::handleBookReturned(const std::string& bookName)
{
    std::string const reqStr = http::request{.type = http::OperationType_e::PUT, .id = getId(), .bookName = bookName}.toString();
    if (conn.Write(reqStr.c_str(), reqStr.size()))
    {
        // TODO: log
    }
    else
    {
        // TODO: log
    }
}
