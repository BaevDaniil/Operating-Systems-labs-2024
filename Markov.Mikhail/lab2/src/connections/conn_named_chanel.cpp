#include "conn_named_chanel.hpp"

NamedChanel::NamedChanel(const std::string & pathname, bool create)
{
    if (create)
        if (mkfifo(pathname.c_str(), 0666) == -1)
            throw std::runtime_error("Failed to create FIFO channel");

    fd = open(pathname.c_str(), O_RDWR);
    if (fd == -1)
        throw std::runtime_error("Failed to open FIFO channel");
}

bool NamedChanel::Read(void *buf, size_t count)
{
    ssize_t bytesRead = read(fd, buf, count);
    if (bytesRead == -1)
        return false;
    return bytesRead > 0;
}

bool NamedChanel::Write(void *buf, size_t count)
{
    ssize_t bytesWritten = write(fd, buf, count);
    if (bytesWritten == -1)
        return false;
    return bytesWritten == (ssize_t)count;
}

NamedChanel::~NamedChanel()
{
    close(fd);
}