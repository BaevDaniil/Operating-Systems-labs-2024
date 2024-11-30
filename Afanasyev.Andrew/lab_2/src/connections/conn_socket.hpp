#pragma once
#include "connection.hpp"
#include <string>
#include <sys/socket.h>

class SocketConnection : public Connection
{
    socklen_t host_socket, client_socket;
    std::string socket_name;
    bool is_host;
    const int max_msg_size = 1024;

public:
    SocketConnection(const std::string &id, bool create);
    bool Read(std::string &) override;
    bool Write(const std::string &) override;
    static std::string to_string() { return "socket"; }
    static std::string create_filename(int num1, int num2)
    {
        return to_string() + std::to_string(num1) + '_' + std::to_string(num2);
    }
    static std::string create_general_filename(int num1, int num2)
    {
        return create_filename(num1, num2) + "_general";
    }
    virtual ~SocketConnection() override;
};