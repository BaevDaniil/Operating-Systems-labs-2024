#pragma once

#include "../includes/includes.hpp"

class ClientInfo
{
private: 
    int pid;
    std::jthread thread;
    std::chrono::time_point<std::chrono::steady_clock> time_point;
};