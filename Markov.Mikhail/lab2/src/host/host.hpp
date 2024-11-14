#pragma once

#include "client_info.hpp"

inline void host_signal_handler(int sig, siginfo_t *info, void *context)
{
    std::cout << "signal was handled" << std::endl;
    switch (sig)
    {
    case SIGUSR1:
        std::cout << info->si_pid << std::endl;
        break;
    
    default:
        std::cout << info->si_pid << std::endl;
        break;
    }
}

template <Conn T>
class Host
{
private:
    int pid;
    struct sigaction signal_handler;
    std::unordered_map<int, ClientInfo<T>> table;
public:
    friend void host_signal_handler(int, siginfo_t *, void *);

    Host(const std::string &pid_path) : pid(getpid()), table()
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
    }
};