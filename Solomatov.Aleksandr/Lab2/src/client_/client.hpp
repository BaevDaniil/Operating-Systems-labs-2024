#pragma once
#include "../connections_/connection.hpp"
#include "../gui/client_gui.hpp"
#include "client_connection_logic.hpp"

class Client {
private:
    ClientMainWindow* window;
    ClientConnectionLogic conlogic;
    std::thread listenThread;
    std::atomic<bool> active;
    std::thread idleTimerThread;
    std::atomic<bool> idle;
    static constexpr int idleTimeoutSeconds = 60;

public:
    Client(ConnectionType type, pid_t host_pid, pid_t client_pid, sighendlerType sig_hendler);

    void set_up_ui(ClientMainWindow *ptr)
    {
        window = ptr;
    }
    
    // void start();
    void handleTimeout();
    void resetIdleTimer();
    void send_to_host(const std::string&);
    void send_to_host_general(const std::string&);
    void process_general_chat();
    void process_host_chat();
    ~Client();
};
