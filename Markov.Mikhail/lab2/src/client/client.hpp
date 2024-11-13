#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

template<Conn T>
class Client
{
private:
    int host_pid;
    int pid;
    T connect;
    struct sigaction signal_handler;
public:
    Client(const std::string& host_pid_path);

    friend void client_signal_handler(int, siginfo_t *, void *);
};

