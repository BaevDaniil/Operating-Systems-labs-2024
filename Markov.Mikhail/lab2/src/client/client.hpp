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
public:
    Client(const std::string& host_pid_path);
};