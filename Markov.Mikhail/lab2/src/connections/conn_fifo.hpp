#pragma once

#include "connection.hpp"

class NamedChannel final : public Connection
{
private:
    std::string pathname;
    const int max_msg_size = 1024;
    int fd;
public:
    
    NamedChannel(const std::string &pathname, bool create);
    bool Read(std::string & message) override;
    virtual bool Write(const std::string & message) override;
    static std::string to_string() { return "fifo"; }
    NamedChannel() = default;
    NamedChannel(const NamedChannel&) = default;
    NamedChannel& operator = (const NamedChannel&) = default;
    NamedChannel(NamedChannel &&) = default;
    NamedChannel &operator=(NamedChannel &&) = default;
    virtual ~NamedChannel() override;
};