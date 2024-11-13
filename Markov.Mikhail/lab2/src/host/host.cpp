#include "host.hpp"

void host_signal_handler(int, siginfo_t *, void *) 
{
    return;
}

template <Conn T>
Host<T>::Host(const std::string &path) : pid(getpid()), table()
{
    std::ofstream file(path);
    if(!file)
        throw std::runtime_error("No config file");
    file << pid;
    file.close();

    signal_handler.sa_sigaction = host_signal_handler;
    signal_handler.sa_flags = SA_SIGINFO;
}