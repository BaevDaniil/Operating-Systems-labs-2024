#pragma once
#include <cstring>
#include <stdexcept>

template<class T>
class Connection
{
public:
    Connection(const T& id, bool create);
    virtual bool Read(void *buf, size_t count) = 0;
    virtual bool Write(void *buf, size_t count) = 0;
    virtual ~Connection() = default;
    Connection() = default;
};