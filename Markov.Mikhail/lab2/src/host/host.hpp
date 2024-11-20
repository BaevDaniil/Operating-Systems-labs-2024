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
    friend void stop_host_signal_handler(int, siginfo_t *, void *);

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

    bool is_client_info_valid(int pid)
    {
        if (!table.contains(pid))
            return false;
        if (!table[pid].is_valid())
        {
            table.erase(pid);
            std::cout <<"delete client with pid: " << pid << std::endl; 
            return false;
        }
        return true;
    }

public:
    static Host &get_instance(const std::string &pid_path, bool create)
    {
        static Host instance(pid_path, create);
        return instance;
    }

    std::vector<int> send_message_to_all_clients_except_one(const std::string& msg, int except_pid)
    {
        std::vector<int> invalid_pids;
        for (auto&& [pid, client_info] : table)
            if (is_client_info_valid(pid) && pid != except_pid)
                client_info.push_general_message(msg);
            else    
                invalid_pids.push_back(pid);
        return invalid_pids;
    }

    std::vector<int> push_message_to_all_clients(const std::string& msg)
    {
        std::vector<int> invalid_pids;
        for (auto &&[pid, client_info] : table)
            if (is_client_info_valid(pid))
            {
                std::cout << "pushed to general msg " << msg << std::endl;
                client_info.push_general_message(msg);
            }
            else
                invalid_pids.push_back(pid);
        return invalid_pids;
    }

    ~Host() 
    {
        std::cout << "host destr" << std::endl;
        stop();
    }

    bool push_message(int pid, const std::string& msg)
    {
        if(is_client_info_valid(pid))
        {
            table[pid].push_message(msg);
            return true;
        }
        return false;
    }

    void stop()
    {
        for (auto &&[pid, client_info] : table)
        {
            kill(pid, SIGKILL);
            client_info.stop();
        }
    }
};

namespace host_namespace
{
    const bool identifier = true;
    const std::filesystem::path host_pid_path = std::filesystem::current_path() / "host/host.txt";
}