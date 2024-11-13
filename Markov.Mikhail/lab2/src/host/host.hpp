#pragma once

#include "../connections/all_connections.hpp"
#include "client_info.hpp"
template <typename Conn>
concept ConnectionType = std::is_base_of<Connection, Conn>::value;

template <typename ConnectionType>
class Host
{
    Conn connection;
    std::filesystem::path pid_path;
    std::unordered_map<std::string, ClientInfo> table;
};