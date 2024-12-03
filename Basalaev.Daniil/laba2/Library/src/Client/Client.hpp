#pragma once

#include "Common/Alias.hpp"
#include "Common/Book.hpp"
#include "Common/NetWorkElementImpl.hpp"
#include "Common/Http.hpp"

#include <atomic>

class ClientWindow;
class QApplication;

// TODO: remove
#include <QApplication>
#include "ClientWindow.hpp"

class Client : public NetWorkElementImpl
{
    Q_OBJECT

public:
    Client(alias::id_t, SemaphoreLocal&, connImpl&, alias::book_container_t const&, QObject* parent = nullptr);
    ~Client() override;

    int start() override;
    void listen() override;

private slots:
    void handleBookSelected(const std::string& bookName, alias::id_t clientId) override;
    void handleBookReturned(const std::string& bookName, alias::id_t clientId) override;

private:
    QApplication m_app;
    ClientWindow m_window;
    std::atomic<bool> m_isRunning{true};
    http::OperationType_e m_lastOpeartion{};
};
