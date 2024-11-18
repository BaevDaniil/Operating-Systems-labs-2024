#pragma once

#include "client_info.hpp"

void host_signal_handler(int, siginfo_t *, void *);

template <Conn T>
class Host
{
private:
    int pid;
    struct sigaction signal_handler;
    std::unordered_map<int, ClientInfo<T>> table; // TODO: add mutex to emplace into table
    friend void host_signal_handler(int, siginfo_t *, void *);

    Host(const std::string &pid_path, bool create) : pid(getpid()), table()
    {
        std::ofstream file(pid_path);
        if (!file)
            throw std::runtime_error("No config file: " + std::string(std::filesystem::absolute(pid_path)));
        file << pid;
        file.close();
        std::cout << "file closed" << std::endl;
        signal_handler.sa_sigaction = host_signal_handler;
        signal_handler.sa_flags = SA_SIGINFO;
        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1)
        {
            throw std::runtime_error("Failed to register signal handler");
        }
        if (sigaction(SIGUSR2, &signal_handler, nullptr) == -1)
        {
            throw std::runtime_error("Failed to register signal handler");
        }
    }
public:
    static Host &get_instance(const std::string &pid_path, bool create)
    {
        static Host instance(pid_path, create);
        return instance;
    }
    std::unordered_map<int, ClientInfo<T>>& get_table()
    {
        return table;
    }

    void send_message_to_all_clients_except_one(const std::string& msg, int except_pid)
    {
        for (auto&& [pid, client_info] : table)
            if (pid != except_pid)
                client_info.send_to_client_general(msg);
    }

    // ~Host()
    // {
    //     for (auto &&[pid, client_info] : table)
    //         kill(pid, SIGKILL); // ctrl C <=> SIGINT does not call destructors. Needs another handler or GUI
    // }
};

namespace host_namespace
{
    const bool identifier = true;
    const std::filesystem::path host_pid_path = std::filesystem::current_path() / "host/host.txt";
}