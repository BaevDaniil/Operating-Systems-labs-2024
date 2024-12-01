#ifndef CONN_H
#define CONN_H

#include "includes.h"

class Conn
{
public:
    Conn() = default;
    virtual ~Conn() = default;
    virtual bool read(const std::string&, unsigned) = 0;
    virtual bool write(const std::string&) = 0;
    virtual bool is_valid() const = 0;
};

#endif // CONN_H
