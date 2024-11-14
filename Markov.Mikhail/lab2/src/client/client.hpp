#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

inline void client_signal_handler(int sig, siginfo_t *info, void *context)
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
class Client
{
private:
    int host_pid;
    int pid;
    T host_to_client;
    T client_to_host;
    struct sigaction signal_handler;
public:
    friend void client_signal_handler(int, siginfo_t *, void *);
    Client(const std::string& host_pid_path)
    {
        std::fstream file(host_pid_path);
        if (!file)
            throw std::runtime_error("No config file" + std::string(std::filesystem::absolute(host_pid_path)));
        std::cout << "File opened successfully." << std::endl;
        file >> host_pid;
        std::cout << "Host PID: " << host_pid << std::endl;
        file.close();
        pid = getpid();

        signal_handler.sa_sigaction = client_signal_handler;
        signal_handler.sa_flags = SA_SIGINFO;

        if (sigaction(SIGUSR1, &signal_handler, nullptr) == -1)
        {
            throw std::runtime_error("Failed to register signal handler");
        }
        
        kill(host_pid, SIGUSR1);
        kill(getpid(), SIGUSR1); // for test
    }

    friend void client_signal_handler(int, siginfo_t *, void *);
};

