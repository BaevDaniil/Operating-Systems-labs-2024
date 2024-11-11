#pragma once

#include "connection.hpp"

class NamedChanel final : public Connection
{
    NamedChanel(const std::string &pathname, bool create);
    virtual bool Read(void *buf, size_t count) override;
    virtual bool Write(void *buf, size_t count) override;
    virtual ~NamedChanel() override;
private:
    int fd;
};