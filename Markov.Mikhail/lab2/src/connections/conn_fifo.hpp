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
    NamedChannel() = default;
    NamedChannel(const NamedChannel&) = delete;
    NamedChannel& operator = (const NamedChannel&) = delete;
    NamedChannel(NamedChannel &&) = default;
    NamedChannel &operator=(NamedChannel &&) = default;
    virtual ~NamedChannel() override;
};