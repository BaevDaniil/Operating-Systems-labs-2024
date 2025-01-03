#pragma once

#include "conn_shm.hpp"
#include "conn_mq.hpp"
#include "conn_pipe.hpp"

enum class ConnectionType
{
    Shm,
    Mq,
    Pipe
};
class ConnectionFabric
{
public:
    static std::unique_ptr<Connection> create_connection(ConnectionType type, const std::string& id, bool create)
    {
        switch (type)
        {
        case ConnectionType::Shm:
            return std::make_unique<ShmConnection>(id, create);
        case ConnectionType::Mq:
            return std::make_unique<MQConnection>(id, create);
        case ConnectionType::Pipe:
            return std::make_unique<PipeConnection>(id, create);
        }
        throw std::logic_error("Undeffinded type error.");
    }
    static std::string create_filename(ConnectionType type, int num1, int num2)
    {
        switch (type)
        {
        case ConnectionType::Shm:
            return ShmConnection::create_filename(num1, num2);
        case ConnectionType::Mq:
            return MQConnection::create_filename(num1, num2);
        case ConnectionType::Pipe:
            return PipeConnection::create_filename(num1, num2);
        }
        return {};
    }
    static std::string create_general_filename(ConnectionType type, int num1, int num2)
    {
        switch (type)
        {
        case ConnectionType::Shm:
            return ShmConnection::create_general_filename(num1, num2);
        case ConnectionType::Mq:
            return MQConnection::create_general_filename(num1, num2);
        case ConnectionType::Pipe:
            return PipeConnection::create_general_filename(num1, num2);
        }
        return {};
    }
};