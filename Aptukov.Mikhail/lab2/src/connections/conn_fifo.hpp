#pragma once

#include "connection.hpp"

class FifoConnection : public Connection
{
private:
    std::string fifo_path;
    static const int MAX_MESSAGE_SIZE = 1024;
    int fifo_fd;
    static std::string typeToString() { return "fifo"; }

public:
    explicit FifoConnection(const std::string &fifo_path, bool create);
    bool Read(std::string & message) override;
    virtual bool Write(const std::string & message) override;
    FifoConnection() = default;
    FifoConnection(const FifoConnection &) = default;
    FifoConnection &operator=(const FifoConnection &) = default;
    FifoConnection(FifoConnection &&) = default;
    FifoConnection &operator=(FifoConnection &&) = default;
    virtual ~FifoConnection() override;

    static std::string GenerateFilename(int num1, int num2)
    {
        return typeToString() + std::to_string(num1) + '_' + std::to_string(num2);
    }

    static std::string GenerateGeneralFilename(int num1, int num2)
    {
        return "general" + GenerateFilename(num1, num2);
    }
};