#pragma once

class connImpl
{
public:
    virtual ~connImpl() {};

    virtual bool Read(void* buf, size_t count) = 0;
    virtual bool Write(const void* buf, size_t count) = 0;

    virtual bool isValid() const = 0;
};
