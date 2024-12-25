#include "conn_sock.hpp"
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdexcept>

SocketConnection::SocketConnection(const std::string &id, bool create) 
    : socketName("/tmp/sock_" + id), isHost(create) {
    struct sockaddr_un address;
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, socketName.c_str(), sizeof(address.sun_path) - 1);

    if (isHost)
    {
        hostSocket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (hostSocket < 0)
            throw std::runtime_error("Host socket creation error");

        if (bind(hostSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
            throw std::runtime_error("Bind socket error");



        clientSocket = accept(hostSocket, nullptr, nullptr);
        if (clientSocket < 0)
            throw std::runtime_error("Accept client socket error");
    }
    else
    {
        clientSocket = socket(AF_UNIX, SOCK_SEQPACKET, 0);
        if (clientSocket < 0)
            throw std::runtime_error("Client socket creation error");

        if (connect(clientSocket, (const struct sockaddr *)&address, sizeof(struct sockaddr_un)) == -1)
            throw std::runtime_error("Connect sockets error");
    }
}

bool SocketConnection::Read(std::string &message)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    
    ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0)
    {
        std::cerr << "Error reading from socket: " << strerror(errno) << std::endl;
        return false;
    }

    message.assign(buffer, bytesReceived);
    return true;
}

bool SocketConnection::Write(const std::string &message)
{
    if (send(clientSocket, message.c_str(), message.size(), MSG_NOSIGNAL) == -1)
    {
        std::cerr << "Error writing to socket: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

SocketConnection::~SocketConnection()
{
    if (isHost)
    {
        close(clientSocket);
        close(hostSocket);
        unlink(socketName.c_str());
    }
    else
    {
        close(clientSocket);
    }
}
