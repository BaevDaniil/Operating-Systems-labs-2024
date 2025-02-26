#pragma once
#include "connection.hpp"
#include <string>
#include <sys/socket.h>

class SocketConnection : public Connection
{
    socklen_t host_socket, client_socket;
    std::string socket_name;
    bool is_host;
    static const int MAX_MESSAGE_SIZE = 1024;
    static std::string typeToString() { return "sock"; }

public:
    explicit SocketConnection(const std::string &id, bool create);
    bool Read(std::string &) override;
    bool Write(const std::string &) override;

    static std::string GenerateFilename(int num1, int num2)
    {
        return typeToString() + std::to_string(num1) + '_' + std::to_string(num2);
    }
    static std::string GenerateGeneralFilename(int num1, int num2)
    {
        return "general" + GenerateFilename(num1, num2);
    }
    virtual ~SocketConnection() override;
};