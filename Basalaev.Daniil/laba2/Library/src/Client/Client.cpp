#include "Client.hpp"
#include "ClientWindow.hpp"
#include "Common/Logger.hpp"

#include <QApplication>
#include <signal.h>
#include <thread>

static int argc = 0;

Client::Client(alias::id_t id, SemaphoreLocal& semaphore, connImpl& connection, alias::book_container_t const& books, QObject* parent)
    : QObject(parent)
    , m_id(id)
    , m_semaphore(semaphore)
    , m_connection(connection)
    , m_books{books}
{}

Client::~Client() = default;

int Client::start()
{
    LOG_INFO(CLIENT_LOG, "[ID=" + std::to_string(getId()) + "] successfully start");

    std::thread listener(&Client::listen, this); // start listen responses from host

    QApplication app(argc, nullptr);
    m_window = std::unique_ptr<ClientWindow>(new ClientWindow(getId(), m_books));

    QObject::connect(m_window.get(), &ClientWindow::bookSelected, this, &Client::handleBookSelected);
    QObject::connect(m_window.get(), &ClientWindow::bookReturned, this, &Client::handleBookReturned);

    m_window->show();
    int res = app.exec();

    m_isRunning = false; // TODO: make stop
    if (listener.joinable()) { listener.join(); }

    return res;
}

void Client::listen()
{
    while (m_isRunning)
    {
        m_semaphore.wait();

        char buffer[alias::MAX_MSG_SIZE] = {0};
        if (m_connection.Read(buffer))
        {
            // Check for notifications
            if (auto notify = http::notification::parse(std::string(buffer)))
            {
                LOG_INFO(CLIENT_LOG, "[ID=" + std::to_string(getId()) + "] successfully read msg from host: " + notify->toString());
                m_books = std::move(notify->books);
                m_window->updateBooks(m_books);
                continue;
            }
            
            if (auto rsp = http::response::parse(std::string(buffer)))
            {
                LOG_INFO(CLIENT_LOG, "[ID=" + std::to_string(getId()) + "] successfully read msg from host: " + rsp->toString());

                if (rsp->id != getId())
                {
                    LOG_ERROR(CLIENT_LOG, "[ID=" + std::to_string(getId()) + "] vs id = " + std::to_string(rsp->id) + " from host's msg");
                    continue;
                }

                std::string const bookName = m_window->getCurrentBook();
                if (m_lastOpeartion == http::OperationType_e::POST)
                {
                    rsp->status == http::OperationStatus_e::OK ? m_window->onSuccessTakeBook(bookName, getId()) : m_window->onFailedTakeBook(bookName, getId());
                }
                else
                {
                    rsp->status == http::OperationStatus_e::OK ? m_window->onSuccessReturnBook(bookName, getId()) : m_window->onFailedReturnBook(bookName, getId());
                }
            }
            else
            {
                LOG_ERROR(CLIENT_LOG, "[ID=" + std::to_string(getId()) + "] read, but failed to parse msg from host");
            }
        }

        m_semaphore.post();
        sleep(0.1);
    }
}

void Client::handleBookSelected(std::string const& bookName, alias::id_t clientId)
{
    std::string const reqStr = http::request{.type = http::OperationType_e::POST, .id = clientId, .bookName = bookName}.toString();
    if (m_connection.Write(reqStr.c_str(), reqStr.size()))
    {
        LOG_INFO(CLIENT_LOG, "[ID=" + std::to_string(clientId) + "] write to host: " + reqStr);
        m_lastOpeartion = http::OperationType_e::POST;
    }
    else
    {
        LOG_ERROR(CLIENT_LOG, "[ID=" + std::to_string(clientId) + "] failed to write to host: " + reqStr);
        m_window->onFailedTakeBook(bookName, clientId);
    }
}

void Client::handleBookReturned(std::string const& bookName, alias::id_t clientId)
{
    std::string const reqStr = http::request{.type = http::OperationType_e::PUT, .id = clientId, .bookName = bookName}.toString();
    if (m_connection.Write(reqStr.c_str(), reqStr.size()))
    {
        LOG_INFO(CLIENT_LOG, "[ID=" + std::to_string(clientId) + "] write to host: " + reqStr);
        m_lastOpeartion = http::OperationType_e::PUT;
    }
    else
    {
        LOG_ERROR(CLIENT_LOG, "[ID=" + std::to_string(clientId) + "] failed to write to host: " + reqStr);
        m_window->onFailedReturnBook(bookName, clientId);
    }
}
