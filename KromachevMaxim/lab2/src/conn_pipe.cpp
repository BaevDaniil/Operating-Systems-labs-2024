#include "conn_pipe.h"

ConnPipe::ConnPipe(int pipe)
{
    this->pipe = pipe;
    int flags = fcntl(pipe, F_GETFL, 0);
    if (flags == -1) {
        std::perror("Ошибка получения флагов сокета");
        return;
    }
    flags |= O_NONBLOCK;
    if (fcntl(pipe, F_SETFL, flags) == -1) {
        std::perror("Ошибка установки неблокирующего режима");
        return;
    }
    valid = true;
}

ConnPipe::~ConnPipe()
{
    close(pipe);
}

bool ConnPipe::read(std::string& buf, unsigned size)
{
    if(!valid)
    {
        std::perror("Канал недоступен");
        return false;
    }
    else
    {
        char read_buf[size];
        memset(read_buf, '\0', size);
        int read_bytes = ::read(pipe, read_buf, size - 1);

        if(read_bytes == -1)
        {
            return false;
        }

        buf.assign(read_buf, read_bytes);
        return true;
    }
}

bool ConnPipe::write(const std::string& buf)
{
    if(!valid) {
        std::perror("Канал недоступен");
        return false;
    }
    else {
        if(::write(pipe, buf.c_str(), buf.size()) == -1) {
            std::perror("Ошибка записи в канал");
            return false;
        }
        return true;
    }
}

bool ConnPipe::is_valid() const
{
    return valid;
}
