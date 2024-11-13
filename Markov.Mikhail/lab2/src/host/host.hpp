#pragma once

#include "client_info.hpp"

template<Conn T>
class Host
{
private:
    int pid;
    std::unordered_map<int, ClientInfo> table;
public: 
    Host(const std::string& pid_path);

};