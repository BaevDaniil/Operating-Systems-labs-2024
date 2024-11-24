#pragma once

#include "ConnectClient.hpp"
#include "HostWindow.hpp"
#include "../connections/all_connections.hpp"
void host_signal_handler(int, siginfo_t *, void *);

template <Conn T>
class Host
{
    int pid;
    std::string pid_path;
    struct sigaction signal_handler;
    std::unordered_map<int, ConnectClient<T>> table;
    friend void host_signal_handler(int, siginfo_t *, void *);
    friend class HostWindow;



    Host(const std::string &pid_path, bool create) : pid(getpid()), pid_path(pid_path), table()
    {
        std::ofstream file(pid_path);
        if (!file)
            throw std::runtime_error("No config file: " + std::string(std::filesystem::absolute(pid_path)));
        file << pid;
        std::cerr << pid << std::endl;
        file.close();
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

    ~Host() 
    {
        remove(pid_path.c_str());
    }

};

namespace host_namespace
{
    const bool identifier = true;
    const std::filesystem::path host_pid_path = std::filesystem::current_path() / "host.pid";
}