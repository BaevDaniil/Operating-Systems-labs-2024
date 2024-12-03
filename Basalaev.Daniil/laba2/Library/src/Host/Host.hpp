#pragma once

#include "Common/Alias.hpp"
#include "Common/Book.hpp"
#include "Common/NetWorkElementImpl.hpp"

#include <atomic>

class HostWindow;
class QApplication;

// TODO: remove
#include <QApplication>
#include "HostWindow.hpp"

class Host : public NetWorkElementImpl
{
    Q_OBJECT

public:
    Host(SemaphoreLocal&, connImpl&, alias::book_container_t const&, QObject* parent = nullptr);
    ~Host() override;

    int start() override;
    void listen() override;

private slots:
    void handleBookSelected(const std::string& bookName, alias::id_t clientId) override;
    void handleBookReturned(const std::string& bookName, alias::id_t clientId) override;

private:
    QApplication m_app; // TODO: remove and make m_window as pointer
    HostWindow m_window;
    std::atomic<bool> m_isRunning{true};
    alias::book_container_t m_books;
    alias::clients_id_container_t m_clients;
};
