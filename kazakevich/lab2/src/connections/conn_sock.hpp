#pragma once
#include "connection.hpp"
#include <string>
#include <sys/socket.h>

class SocketConnection final : public Connection
{
    socklen_t hostSocket, clientSocket;
    std::string socketName;
    bool isHost;

public:
    SocketConnection(const std::string &id, bool create);
    bool Read(std::string &) override;
    bool Write(const std::string &) override;
    static std::string to_string() { return "socket"; }
    static std::string make_filename(int pid1, int pid2)
    {
        return to_string() + '_' + std::to_string(pid1) + '_' + std::to_string(pid2);
    }
    static std::string make_general_filename(int pid1, int pid2)
    {
        return make_filename(pid1, pid2) + "_general";
    }
    virtual ~SocketConnection() override;
};