#pragma once

#include "../includes/includes.hpp"
#include "../connections/all_connections.hpp"
class ClientInfo
{
private: 
    int pid;
    std::jthread thread;
    std::chrono::time_point<std::chrono::steady_clock> time_point;
};