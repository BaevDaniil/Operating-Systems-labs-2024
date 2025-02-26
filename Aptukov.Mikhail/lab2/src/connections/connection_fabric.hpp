#pragma once

#include "conn_fifo.hpp"
#include "conn_mq.hpp"
#include "conn_socket.hpp"

enum class ConnectionType
{
    Fifo,
    Mq,
    Socket
};
class ConnectionFabric
{
public:
    static std::unique_ptr<Connection> CreateConnection(ConnectionType type, const std::string& id, bool create)
    {
        switch (type)
        {
        case ConnectionType::Fifo:
            return std::make_unique<FifoConnection>(id, create);
        case ConnectionType::Mq:
            return std::make_unique<MQConnection>(id, create);
        case ConnectionType::Socket:
            return std::make_unique<SocketConnection>(id, create);
        default:
            throw std::logic_error("Unknown connection type");
        }
    }
    static std::string GenerateFilename(ConnectionType type, int num1, int num2)
    {
        switch (type)
        {
        case ConnectionType::Fifo:
            return FifoConnection::GenerateFilename(num1, num2);
        case ConnectionType::Mq:
            return MQConnection::GenerateFilename(num1, num2);
        case ConnectionType::Socket:
            return SocketConnection::GenerateFilename(num1, num2);
        default:
            throw std::logic_error("Unknown connection type");
        }
    }
    static std::string GenerateGeneralFilename(ConnectionType type, int num1, int num2)
    {
        switch (type)
        {
        case ConnectionType::Fifo:
            return FifoConnection::GenerateGeneralFilename(num1, num2);
        case ConnectionType::Mq:
            return MQConnection::GenerateGeneralFilename(num1, num2);
        case ConnectionType::Socket:
            return SocketConnection::GenerateGeneralFilename(num1, num2);
        default:
            throw std::logic_error("Unknown connection type");
        }
    }
};