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
    static std::unique_ptr<Connection> create_connection(ConnectionType type, const std::string& id, bool create)
    {
        switch (type)
        {
        case ConnectionType::Fifo:
            return std::make_unique<FifoConnection>(id, create);
        case ConnectionType::Mq:
            return std::make_unique<MQConnection>(id, create);
        case ConnectionType::Socket:
            return std::make_unique<SocketConnection>(id, create);
        }
        throw std::logic_error("No type!");
    }
    static std::string create_filename(ConnectionType type, int num1, int num2)
    {
        switch (type)
        {
        case ConnectionType::Fifo:
            return FifoConnection::create_filename(num1, num2);
        case ConnectionType::Mq:
            return MQConnection::create_filename(num1, num2);
        case ConnectionType::Socket:
            return SocketConnection::create_filename(num1, num2);
        }
        return {};
    }
    static std::string create_general_filename(ConnectionType type, int num1, int num2)
    {
        switch (type)
        {
        case ConnectionType::Fifo:
            return FifoConnection::create_general_filename(num1, num2);
        case ConnectionType::Mq:
            return MQConnection::create_general_filename(num1, num2);
        case ConnectionType::Socket:
            return SocketConnection::create_general_filename(num1, num2);
        }
        return {};
    }
};