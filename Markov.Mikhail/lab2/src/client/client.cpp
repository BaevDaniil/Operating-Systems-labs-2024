#include "client.hpp"

void client_signal_handler(int, siginfo_t *, void *)
{
    return;
}

template<Conn T>
Client<T>::Client(const std::string& host_pid_path)
{
    std::ifstream file(host_pid_path);
    if (!file)
        throw std::runtime_error("No config file");
    file >> host_pid;
    file.close();
    pid = getpid();

    signal_handler.sa_sigaction = client_signal_handler;
    signal_handler.sa_flags = SA_SIGINFO;

    union sigval value;
    value.sival_int = pid;

    if (sigqueue(host_pid, SIGRTMIN, value) == -1)
        throw std::runtime_error("No config file");
}