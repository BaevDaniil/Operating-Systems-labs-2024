#pragma once

#include "client_logic.hpp"
#include "client_gui.hpp"

class Client
{
private:
    ClientLogic& client_logic_ref;
    ClientMainWindow* window;
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
    Client(Client &&) = delete;
    Client &operator=(Client &&) = delete;

    Client(ClientLogic &ref) : client_logic_ref(ref) {}

public:
    friend void client_signal_handler(int, siginfo_t *, void *);
    static Client& get_instance(ConnectionType type, const std::string& host_pid_path)
    {
        ClientLogic& client_logic = ClientLogic::get_instance(type, host_pid_path);
        static Client instance(client_logic);
        return instance;
    }

    void set_up_ui(ClientMainWindow *ptr)
    {
        window = ptr;
    }

    void process_chat()
    {   
        std::string msg;
        client_logic_ref.read_from_host(msg);
        window->setMessageToChat(msg);
    }

    void process_general_chat()
    {
        std::string msg;
        client_logic_ref.read_from_host_general(msg);
        window->setMessageToGeneralChat(msg);
    }

    void send_to_host_general(const std::string& msg)
    {
        client_logic_ref.send_to_host_general(msg);
    }
    void send_to_host(const std::string &msg)
    {
        client_logic_ref.send_to_host(msg);
    }
};
