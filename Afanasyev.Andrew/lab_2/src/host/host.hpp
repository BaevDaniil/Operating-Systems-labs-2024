#pragma once

#include "host_logic.hpp"
#include "../gui/chat_gui.hpp"

class Host
{
private:
    HostLogic &host_logic_ref;
    MainWindow* window;
    Host(const Host &) = delete;
    Host &operator=(const Host &) = delete;
    Host(Host &&) = delete;
    Host &operator=(Host &&) = delete;

    Host(HostLogic &ref) : host_logic_ref(ref) {}

public:
    friend void client_signal_handler(int, siginfo_t *, void *);
    static Host &get_instance(ConnectionType type, int host_pid, int client_pid)
    {
        HostLogic &client_logic = HostLogic::get_instance(type, host_pid, client_pid);
        static Host instance(client_logic);
        return instance;
    }

    void set_up_ui(MainWindow* ptr)
    {
        window = ptr;
    }

    void process_chat()
    {
        std::string msg;
        bool f = host_logic_ref.read_from_client(msg);
        if(!f)
            window->no_client_notification();
        window->set_msg_to_chat(msg);
    }
    void send_to_client(const std::string &msg)
    {
        bool f = host_logic_ref.send_to_client(msg);
        if (!f)
            window->no_client_notification();
    }
};
