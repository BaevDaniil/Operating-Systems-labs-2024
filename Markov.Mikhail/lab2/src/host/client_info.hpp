#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"

template <Conn T>
class ClientInfo
{
private: 
    int host_pid;
    int pid;
    // T host_to_client;
    // T client_to_host;
    // T host_to_client_general;
    // T client_to_host_general;
    std::vector<T> connections;
    // std::jthread thread;
    // std::chrono::time_point<std::chrono::steady_clock> time_point;
    // void run();
public:
    ClientInfo(int host_pid, int pid, bool create = true) : host_pid(host_pid), pid(pid)
    {
        connections.emplace_back(T::make_filename(host_pid, pid), create);
        connections.emplace_back(T::make_filename(pid, host_pid), create);
        connections.emplace_back(T::make_general_filename(host_pid, pid), create);
        connections.emplace_back(T::make_general_filename(pid, host_pid), create);
    }
    // void start();
};