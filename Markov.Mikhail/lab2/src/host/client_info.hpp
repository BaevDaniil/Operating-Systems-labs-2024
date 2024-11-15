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
        connections.emplace_back(T::to_string() + '_' + std::to_string(host_pid) + "_" + std::to_string(pid), create);
        connections.emplace_back(T::to_string() + '_' + std::to_string(pid) + '_' + std::to_string(host_pid), create);
        connections.emplace_back(T::to_string() + '_' + std::to_string(host_pid) + '_' + std::to_string(pid) + "_" + "general", create);
        connections.emplace_back(T::to_string() + '_' + std::to_string(pid) + '_' + std::to_string(host_pid) + "_" + "general", create);
    }
    // void start();
};