#pragma once
#include "../connections_/connection.hpp"
#include "../gui/client_gui.hpp"
#include "client_connection_logic.hpp"

class Client {
private:
    std::shared_ptr<ClientMainWindow> window;
    std::unique_ptr<ClientConnectionLogic> conlogic;
    std::atomic<bool> active;
    std::thread idleTimerThread;
    std::atomic<bool> idle;
    static constexpr int idleTimeoutSeconds = 15;
    
public:
    Client(ConnectionType type, pid_t host_pid, pid_t client_pid, sighendlerType sig_hendler);

    void set_up_ui(std::shared_ptr<ClientMainWindow> ptr)
    {
        window = ptr;
    }
    
    void handleTimeout();
    void resetIdleTimer();
    void send_to_host(const std::string &msg)
    {
        conlogic->send_to_host(msg);
    }
    void send_to_host_general(const std::string& msg){
        conlogic->send_to_host_general(msg);
    }
    void process_general_chat();
    void process_host_chat();
    void procces_quit();
    ~Client();
};
