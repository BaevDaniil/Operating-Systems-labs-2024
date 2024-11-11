#pragma once

#include "../includes/includes.hpp"

template<typename T>
class Connection
{
public:
    Connection() = default;
    Connection(const T& id, bool create);
    virtual bool Read(void *buf, size_t count) = 0;
    virtual bool Write(void *buf, size_t count) = 0;
    virtual ~Connection() = default;
    Connection() = default;
};