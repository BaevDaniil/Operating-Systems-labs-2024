#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"
class ClientInfo
{
private: 
    int host_pid;
    int client_pid;
    struct sigaction signal_handler;
    std::jthread thread;
    std::chrono::time_point<std::chrono::steady_clock> time_point;
};