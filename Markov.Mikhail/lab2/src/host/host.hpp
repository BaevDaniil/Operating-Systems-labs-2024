#pragma once

#include "client_info.hpp"

template<Conn T>
class Host
{
private:
    int pid;
    struct sigaction signal_handler;
    std::unordered_map<int, ClientInfo> table;
public: 
    Host(const std::string& pid_path);

    friend void host_signal_handler(int, siginfo_t *, void *);
};