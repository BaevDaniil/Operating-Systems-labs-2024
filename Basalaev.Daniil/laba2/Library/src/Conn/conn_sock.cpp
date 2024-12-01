#include "conn_sock.hpp"
#include "Common/Logger.hpp"

#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

std::unique_ptr<ConnSock> ConnSock::crateHostSocket(alias::port_t hostPort)
{
    ConnSock* socket = new ConnSock();
    socket->m_socketFileDesriptor = ::socket(AF_INET, SOCK_STREAM, 0);

    if (!socket->IsInitialized())
    {
        LOG_ERROR("ConnSock", "Host failed to create socket");
        return nullptr;
    }

    socket->m_address.sin_family = AF_INET;
    socket->m_address.sin_port = htons(hostPort);
    socket->m_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(socket->m_socketFileDesriptor, (struct sockaddr*)&socket->m_address, sizeof(socket->m_address)) == -1)
    {
        LOG_ERROR("ConnSock", "Host failed to bind socket");
        close(socket->m_socketFileDesriptor);
        socket->m_socketFileDesriptor = -1;
        return nullptr;
    }

    if (listen(socket->m_socketFileDesriptor, 1) == -1) {
        LOG_ERROR("ConnSock", "Host failed to listen socket");
        close(socket->m_socketFileDesriptor);
        socket->m_socketFileDesriptor = -1;
        return nullptr;
    }

    return std::unique_ptr<ConnSock>(socket);
}

std::unique_ptr<ConnSock> ConnSock::crateClientSocket(alias::port_t hostPort)
{
    ConnSock* socket = new ConnSock();
    socket->m_socketFileDesriptor = ::socket(AF_INET, SOCK_STREAM, 0);

    if (!socket->IsInitialized())
    {
        LOG_ERROR("ConnSock", "Client failed to create socket");
        return nullptr;
    }

    socket->m_address.sin_family = AF_INET;
    socket->m_address.sin_port = htons(hostPort);

    if (inet_pton(AF_INET, "127.0.0.1", &socket->m_address.sin_addr) <= 0)
    {
        LOG_ERROR("ConnSock", "Client recieve invalid host IP address");
        close(socket->m_socketFileDesriptor);
        socket->m_socketFileDesriptor = -1;
        return nullptr;
    }

    if (connect(socket->m_socketFileDesriptor, (struct sockaddr*)&socket->m_address, sizeof(socket->m_address)) == -1)
    {
        LOG_ERROR("ConnSock", "Client failed to connect to host");
        close(socket->m_socketFileDesriptor);
        socket->m_socketFileDesriptor = -1;
        return nullptr;
    }

    return std::unique_ptr<ConnSock>(socket);
}

std::unique_ptr<ConnSock> ConnSock::Accept()
{
    alias::address_t clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    alias::desriptor_t clientFd = accept(m_socketFileDesriptor, (struct sockaddr*)&clientAddr, &clientLen);
    if (clientFd == -1)
    {
        LOG_ERROR("ConnSock", "Host failed to accept connection");
        return nullptr;
    }

    ConnSock* socket = new ConnSock();
    socket->m_socketFileDesriptor = clientFd;
    socket->m_address = clientAddr;
    return std::unique_ptr<ConnSock>(socket);
}

bool ConnSock::Read(void* buf, size_t count)
{
    if (!IsInitialized()) { return false; }

    if (recv(m_socketFileDesriptor, buf, count, 0) <= 0) { return false; }

    return true;
}

bool ConnSock::Write(const void* buf, size_t count)
{
    if (!IsInitialized()) { return false; }

    if (send(m_socketFileDesriptor, buf, count, 0) <= 0) { return false; }

    return true;
}

ConnSock::~ConnSock()
{
    if (IsInitialized()) 
    {
        close(m_socketFileDesriptor);
    }
}

bool ConnSock::IsInitialized() const
{
    return m_socketFileDesriptor != -1;
}
