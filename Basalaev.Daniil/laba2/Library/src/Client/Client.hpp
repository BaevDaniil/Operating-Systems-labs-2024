#pragma once

#include "ClientWindow.hpp"
#include "Common/Alias.hpp"
#include "Common/Book.hpp"
#include "Common/Http.hpp"
#include "Common/SemaphoreLocal.hpp"
#include "Conn/conn_impl.hpp"

#include <atomic>

class Client : public QObject
{
    Q_OBJECT

public:
    Client(alias::id_t, SemaphoreLocal&, connImpl&, alias::book_container_t const&, QObject* parent = nullptr);
    ~Client();

    int start();
    void listen();

    alias::id_t getId() const noexcept { return m_id; }

private slots:
    void handleBookSelected(std::string const& bookName, alias::id_t clientId);
    void handleBookReturned(std::string const& bookName, alias::id_t clientId);

private:
    std::unique_ptr<ClientWindow> m_window{nullptr};
    std::atomic<bool> m_isRunning{true};
    http::OperationType_e m_lastOpeartion{};
    alias::id_t m_id;
    SemaphoreLocal& m_semaphore;
    connImpl& m_connection;
    alias::book_container_t m_books;
};
