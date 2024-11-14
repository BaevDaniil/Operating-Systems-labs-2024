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
    bool Read(std::string & message) override
    {
        char buffer[max_msg_size];
        memset(buffer, '\0', max_msg_size);

        ssize_t bytes_read = read(fd, buffer, max_msg_size - 1);
        if (bytes_read == -1)
            return false;

        message.assign(buffer, bytes_read);
        return true;
    }
    virtual bool Write(const std::string & message) override
    {
        ssize_t bytesWritten = write(fd, message.c_str(), message.size());
        if (bytesWritten == -1)
            return false;
        return true;
    }
    NamedChannel() = default;
    NamedChannel(const NamedChannel&) = delete;
    NamedChannel& operator = (const NamedChannel&) = delete;
    NamedChannel(NamedChannel &&) = default;
    NamedChannel &operator=(NamedChannel &&) = default;
    virtual ~NamedChannel() override
    {
        close(fd);
        unlink(pathname.c_str());
    }
};