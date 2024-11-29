#pragma once

#include "../includes/includes.hpp"

class Connection
{
public:
    virtual bool Read(std::string&) = 0;
    virtual bool Write(const std::string&) = 0;
    virtual ~Connection() = default;
    Connection() = default;
};

template <typename T>
concept Conn = std::is_base_of<Connection, T>::value;