#pragma once
#include "../connections_/connection.hpp"

class Client {
private:
    std::unique_ptr<Connection> connection;
    std::thread listenThread;
    std::atomic<bool> active;
    std::thread idleTimerThread;
    std::atomic<bool> idle;
    static constexpr int idleTimeoutSeconds = 60;

public:
    Client(const std::string &connectionId, bool create, std::unique_ptr<Connection> conn);

    void start();
    void sendMessage(const std::string &message);
    void listen();
    void handleTimeout();
    void resetIdleTimer();

    ~Client();
};
