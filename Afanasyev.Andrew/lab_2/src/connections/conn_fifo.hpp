#pragma once

#include "connection.hpp"

class FifoConnection : public Connection
{
private:
    std::string pathname;
    const int max_msg_size = 1024;
    int fd;
public:
    FifoConnection(const std::string &pathname, bool create);
    bool Read(std::string & message) override;
    virtual bool Write(const std::string & message) override;
    static std::string to_string() { return "fifo"; }
    FifoConnection() = default;
    FifoConnection(const FifoConnection &) = default;
    FifoConnection &operator=(const FifoConnection &) = default;
    FifoConnection(FifoConnection &&) = default;
    FifoConnection &operator=(FifoConnection &&) = default;
    virtual ~FifoConnection() override;

    static std::string create_filename(int num1, int num2)
    {
        return to_string() + std::to_string(num1) + '_' + std::to_string(num2);
    }

    static std::string create_general_filename(int num1, int num2)
    {
        return create_filename(num1, num2) + "general";
    }
};