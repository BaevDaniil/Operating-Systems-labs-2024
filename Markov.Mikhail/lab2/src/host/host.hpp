#pragma once

#include "../connections/all_connections.hpp"
#include "client_info.hpp"
template <typename Conn, typename Key>
concept ConnectionType = std::is_base_of<Connection<Key>, Conn>::value;

template <typename ConnectionType>
class Host
{
    Conn connection;
    static const std::filesystem::path pid_path = "/var/run/host.pid";
    std::unordered_map<std::string, ClientInfo> table;
};