#include "Host.hpp"
#include "HostWindow.hpp"
#include "Common/Http.hpp"
#include "Common/Logger.hpp"

#include <QApplication>
#include <signal.h>
#include <cstdlib>

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

Host::Host(SemaphoreLocal& semaphore, std::vector<alias::id_t> const& clientId, std::vector<std::unique_ptr<connImpl>> connections, alias::book_container_t const& books, QObject* parent)
    : QObject(parent)
    , m_semaphore(semaphore)
    , m_connections(std::move(connections))
    , m_books{books}
{
    for (auto const id : clientId)
    {
        auto& clientInfo = m_clients.emplace_back(utils::ClientInfoWithTimer{
                                .info = {.clientId = id},
                                .timer = std::unique_ptr<QTimer>{new QTimer()}
                            });
        clientInfo.timer->setInterval(1000);
        clientInfo.timer->setSingleShot(false);
        // update client information every 1 second
        connect(clientInfo.timer.get(), &QTimer::timeout, this, [this, &clientInfo]() {
            clientInfo.info.secondsToKill--;
            m_window->updateClientsInfo(m_clients);
            if (clientInfo.info.secondsToKill == 0)
            {
                removeClient(clientInfo.info.clientId);
                m_window->notifyClientTerminated(clientInfo.info.clientId);
            }
        });

        clientInfo.timer->start();
    }
}

Host::~Host() = default;

void Host::stop()
{
    m_isRunning = false;

    for (auto& thread : m_listenerThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

int Host::start()
{
    LOG_INFO(HOST_LOG, "successfully start");

    // start listen messages from clients
    for (auto& connection : m_connections)
    {
        m_listenerThreads.emplace_back(&Host::listen, this, std::ref(*connection));
    }

    int argc = 0;
    QApplication app(argc, nullptr);
    m_window = std::unique_ptr<HostWindow>(new HostWindow("HOST WINDOW", m_books));
    m_window->show();
    m_window->updateClientsInfo(m_clients);
    int res = app.exec();

    stop();

    return res;
}

void Host::listen(connImpl& connection)
{
    while (m_isRunning)
    {
        m_semaphore.wait();

        char buffer[1024] = {0};
        if (connection.Read(buffer, sizeof(buffer)))
        {
            if (auto req = http::request::parse(std::string(buffer)))
            {
                LOG_INFO(HOST_LOG, "successfully read msg from client[ID=" + std::to_string(req->id) + "]: " + req->toString());

                if (req->type == http::OperationType_e::POST)
                {
                    handleBookSelected(req->bookName, req->id, connection);
                }
                else
                {
                    handleBookReturned(req->bookName, req->id, connection);
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

void Host::updateClientInfo(utils::ClientInfo&& clientInfo)
{
    auto it = std::find_if(m_clients.begin(), m_clients.end(), [id = clientInfo.clientId](auto const& client){ return client.info.clientId == id; });
    if (it != m_clients.end())
    {
        it->info = std::move(clientInfo);
    }
}

void Host::resetClientTimer(alias::id_t clientId)
{
    auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientId](auto const& client) {
                               return client.info.clientId == clientId;
                           });

    if (it != m_clients.end())
    {
        it->timer->stop();
        it->timer->start(5000);
        it->info.secondsToKill = 5;
    }
}

void Host::stopClientTimer(alias::id_t clientId)
{
    auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientId](auto const& client) {
                               return client.info.clientId == clientId;
                           });

    if (it != m_clients.end())
    {
        it->timer->stop();
        it->info.secondsToKill = 5;
    }
}

void Host::removeClient(alias::id_t clientId)
{
    LOG_INFO(HOST_LOG, "Removing client[ID=" + std::to_string(clientId) + "]");

    auto it = std::find_if(m_clients.begin(), m_clients.end(), [clientId](auto const& client) {
                                 return client.info.clientId == clientId;
                             });

    if (it != m_clients.end())
    {
        // TODO: stop timer???
        m_clients.erase(it);
    }

    // TODO: close connection
}

void Host::notifyClientsUpdateBookStatus()
{
    http::notification notification{.books = m_books};
    std::string const notificationStr = notification.toString();
    for (auto& connection : m_connections)
    {
        if (connection->Write(notificationStr.c_str(), notificationStr.size())) // w/o sync
        {
            LOG_INFO(HOST_LOG, "write to client: " + notificationStr);
        }
        else
        {
            LOG_ERROR(HOST_LOG, "failed to write to client: " + notificationStr);
        }
    }
}

void Host::handleBookSelected(std::string const& bookName, alias::id_t clientId, connImpl& connection)
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
    if (connection.Write(rspStr.c_str(), rspStr.size()))
    {
        LOG_INFO(HOST_LOG, "write to client: " + rspStr);
        if (rsp.status == http::OperationStatus_e::OK)
        {
            m_window->onSuccessTakeBook(bookName, clientId);
            m_window->updateBooks(m_books);
            updateClientInfo({.clientId = clientId, .readingBook = bookName, .secondsToKill = 5});
            m_window->updateClientsInfo(m_clients);
            // notifyClientsUpdateBookStatus();
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

void Host::handleBookReturned(std::string const& bookName, alias::id_t clientId, connImpl& connection)
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
    if (connection.Write(rspStr.c_str(), rspStr.size()))
    {
        LOG_INFO(HOST_LOG, "write to client: " + rspStr);
        if (rsp.status == http::OperationStatus_e::OK)
        {
            m_window->onSuccessReturnBook(bookName, clientId);
            m_window->updateBooks(m_books);
            updateClientInfo({.clientId = clientId, .readingBook = "", .secondsToKill = 5});
            m_window->updateClientsInfo(m_clients);
            // notifyClientsUpdateBookStatus();
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
