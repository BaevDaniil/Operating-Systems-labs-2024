#pragma once

#include "../includes/includes.hpp"

class Connection
{
public:
    Connection(const std::string &id, bool create);
    virtual bool Read(void *buf, size_t count) = 0;
    virtual bool Write(void *buf, size_t count) = 0;
    virtual ~Connection() = default;
    Connection() = default;
};