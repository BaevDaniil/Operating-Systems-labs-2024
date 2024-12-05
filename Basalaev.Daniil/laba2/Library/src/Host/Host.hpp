#pragma once

#include "Common/Alias.hpp"
#include "Common/Book.hpp"
#include "Common/NetWorkElementImpl.hpp"
#include "HostWindow.hpp"

#include <atomic>

class Host : public NetWorkElementImpl
{
    Q_OBJECT

public:
    Host(SemaphoreLocal&, connImpl&, alias::book_container_t const&, QObject* parent = nullptr);
    ~Host() override;

    int start() override;
    void listen() override;

private slots:
    void handleBookSelected(std::string const& bookName, alias::id_t clientId) override;
    void handleBookReturned(std::string const& bookName, alias::id_t clientId) override;

private:
    void updateClientInfo(utils::ClientInfo);

    std::unique_ptr<HostWindow> m_window{nullptr};
    std::atomic<bool> m_isRunning{true};
    alias::book_container_t m_books;
    std::vector<utils::ClientInfo> m_clients;
};
