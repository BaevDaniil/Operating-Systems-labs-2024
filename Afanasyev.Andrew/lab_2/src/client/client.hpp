#pragma once

#include "client_logic.hpp"
#include "../gui/chat_gui.hpp"

class Client
{
private:
    ClientLogic& client_logic_ref;
    MainWindow* window;
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
    Client(Client &&) = delete;
    Client &operator=(Client &&) = delete;

    Client(ClientLogic &ref) : client_logic_ref(ref) {}

public:
    friend void client_signal_handler(int, siginfo_t *, void *);
    static Client& get_instance(ConnectionType type, int host_pid, int client_pid)
    {
        ClientLogic& client_logic = ClientLogic::get_instance(type, host_pid, client_pid);
        static Client instance(client_logic);
        return instance;
    }

    void set_up_ui(MainWindow *ptr)
    {
        window = ptr;
    }

    void process_chat()
    {   
        std::string msg;
        client_logic_ref.read_from_host(msg);
        window->set_msg_to_chat(msg);
    }
    void send_to_host(const std::string &msg)
    {
        client_logic_ref.send_to_host(msg);
    }
};
