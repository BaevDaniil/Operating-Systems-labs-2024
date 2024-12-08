#pragma once

#include "Common/Alias.hpp"
#include "Common/Book.hpp"
#include "Common/SemaphoreLocal.hpp"
#include "Conn/conn_impl.hpp"
#include "HostWindow.hpp"

#include <atomic>
#include <thread>

class Host : public QObject
{
    Q_OBJECT

public:
    Host(SemaphoreLocal&, std::vector<alias::id_t> const&, std::vector<std::unique_ptr<connImpl>>, alias::book_container_t const&, QObject* parent = nullptr);
    ~Host();

    int start();
    void listen(connImpl&);
    void stop();

private slots:
    void handleBookSelected(std::string const& bookName, alias::id_t clientId, connImpl&);
    void handleBookReturned(std::string const& bookName, alias::id_t clientId, connImpl&);

private:
    void updateClientInfo(utils::ClientInfo&&);
    void resetClientTimer(alias::id_t);
    void stopClientTimer(alias::id_t);
    void removeClient(alias::id_t);
    void notifyClientsUpdateBookStatus();

    SemaphoreLocal& m_semaphore;
    std::vector<std::unique_ptr<connImpl>> m_connections;
    std::unique_ptr<HostWindow> m_window{nullptr};
    std::atomic<bool> m_isRunning{true};
    alias::book_container_t m_books;
    std::vector<utils::ClientInfoWithTimer> m_clients;
    std::vector<std::thread> m_listenerThreads;
};
