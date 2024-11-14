#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

template <Conn T>
class ClientInfo
{
private: 
    int host_pid;
    int client_pid;
    std::jthread thread;
    std::chrono::time_point<std::chrono::steady_clock> time_point;
    T host_to_client;
    T client_to_host;
    // void run();
public:
    std::atomic<int> read_times = 0;
    std::atomic<int> write_times = 0;
    // void start();
};