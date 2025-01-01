#pragma once

#include "client_info.hpp"

void host_signal_handler(int, siginfo_t *, void *);

class Host;
class HostLogic
{
private:
    int pid;
    struct sigaction signal_handler;
    std::unordered_map<int, ClientInfo> table;
    friend class Host;
    friend void host_signal_handler(int, siginfo_t *, void *);

    HostLogic(const std::string &pid_path) : pid(getpid()), table()
    {
        std::ofstream file(pid_path);
        if (!file)
            throw std::runtime_error("No config file: " + std::string(std::filesystem::absolute(pid_path)));
        file << pid;
        file.close();
        signal_handler.sa_sigaction = host_signal_handler;
        signal_handler.sa_flags = SA_SIGINFO;
        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1 ||
            sigaction(SIGUSR2, &signal_handler, nullptr) == -1)
            throw std::runtime_error("Failed to register signal handler");
    }

    bool is_client_info_valid(int pid)
    {
        return table[pid].is_valid();
    }

public:
    HostLogic(const HostLogic &) = delete;
    HostLogic &operator=(const HostLogic &) = delete;
    HostLogic(HostLogic &&) = delete;
    HostLogic &operator=(HostLogic &&) = delete;
    static HostLogic &get_instance(const std::string &pid_path)
    {
        static HostLogic instance(pid_path);
        return instance;
    }

    std::vector<int> send_message_to_all_clients_except_one(const std::string& msg, int except_pid)
    {
        std::vector<int> invalid_pids;
        for (auto&& [pid, client_info] : table)
            if (is_client_info_valid(pid) )
            {
                if(pid != except_pid)
                    client_info.push_general_message(msg);
            }
            else    
                invalid_pids.push_back(pid);
        for (int i : invalid_pids)
        {
            table.erase(i);
        }
        return invalid_pids;
    }

    std::vector<int> push_message_to_all_clients(const std::string& msg)
    {
        std::vector<int> invalid_pids;
        for (auto &&[pid, client_info] : table)
            if (is_client_info_valid(pid))
                client_info.push_general_message(msg);
            else
                invalid_pids.push_back(pid);
        for(int i : invalid_pids)
        {
            table.erase(i);
        }
        return invalid_pids;
    }

    ~HostLogic() 
    {
        stop();
    }

    bool push_message(int pid, const std::string& msg)
    {
        if(is_client_info_valid(pid))
        {
            table[pid].push_message(msg);
            return true;
        }
        table.erase(pid);
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