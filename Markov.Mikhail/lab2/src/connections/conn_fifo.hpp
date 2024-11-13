#pragma once

#include "connection.hpp"

class NamedChanel final : public Connection
{
    NamedChanel(const std::string &pathname, bool create);
    virtual bool Read(std::string&) override;
    virtual bool Write(const std::string&) override;
    virtual ~NamedChanel() override;
private:
    std::string pathname;
    const int max_msg_size = 1024;
    int fd;
};