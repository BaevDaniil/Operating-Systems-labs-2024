#pragma once

#include "host_logic.hpp"
#include "host_gui.hpp"

class Host
{
private:
    HostLogic& host_logic_ref;
    ConnectionType type;
    MainWindow* window;
    Host(const Host &) = delete;
    Host &operator=(const Host &) = delete;
    Host(Host &&) = delete;
    Host &operator=(Host &&) = delete;

    Host(ConnectionType type, const std::string &host_pid_path) : host_logic_ref(HostLogic::get_instance(host_pid_path)), type(type) {}

public:
    friend void host_signal_handler(int, siginfo_t *, void *);
    static Host &get_instance(ConnectionType type, const std::string& host_pid_path)
    {
        static Host instance(type, host_pid_path);
        return instance;
    }

    void process_chat(int pid)
    {
        host_logic_ref.table[pid].append_unread_counter();
    }

    void process_general_chat(int pid)
    {
        std::string msg;
        host_logic_ref.table[pid].read_from_client_general(msg);
        window->set_msg_to_general_chat(msg);
        std::vector<int> invalid_pids = host_logic_ref.send_message_to_all_clients_except_one(msg, pid);
        for(int i: invalid_pids)
            window->remove_client(i);
    }

    bool create_new_client(int pid)
    {
        if (!host_logic_ref.table.contains(pid))
        {
            host_logic_ref.table.emplace(pid, std::move(ClientInfo{type, getpid(), pid}));
            auto f = [](MainWindow *window_pointer, int pid, const std::string &msg)
            {
                window_pointer->set_msg_to_chat(pid, msg);
            };
            host_logic_ref.table[pid].start(window, f);
            window->add_client(pid);
            return true;
        }
        return false;
    }

    void set_up_ui(MainWindow* ptr)
    {
        window = ptr;
    }

    std::vector<int> push_message_to_all_clients(const std::string &msg)
    {
        return host_logic_ref.push_message_to_all_clients(msg);
    }

    bool push_message(int pid, const std::string& msg)
    {
        return host_logic_ref.push_message(pid, msg);
    }
};