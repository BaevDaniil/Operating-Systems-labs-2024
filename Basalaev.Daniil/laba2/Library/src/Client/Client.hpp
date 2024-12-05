#pragma once

#include "ClientWindow.hpp"
#include "Common/Alias.hpp"
#include "Common/Book.hpp"
#include "Common/NetWorkElementImpl.hpp"
#include "Common/Http.hpp"

#include <atomic>

class Client : public NetWorkElementImpl
{
    Q_OBJECT

public:
    Client(alias::id_t, SemaphoreLocal&, connImpl&, alias::book_container_t const&, QObject* parent = nullptr);
    ~Client() override;

    int start() override;
    void listen() override;

private slots:
    void handleBookSelected(std::string const& bookName, alias::id_t clientId) override;
    void handleBookReturned(std::string const& bookName, alias::id_t clientId) override;

private:
    std::unique_ptr<ClientWindow> m_window{nullptr};
    std::atomic<bool> m_isRunning{true};
    http::OperationType_e m_lastOpeartion{};
    alias::book_container_t m_books; // TODO: try to remove when notification be implemented
};
