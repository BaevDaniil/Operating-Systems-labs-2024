#include "conn_fifo.h"

ConnFifo::ConnFifo(const std::string& path, bool create): path(path)
{
    setup_conn(create);
}

ConnFifo::~ConnFifo()
{
    if(valid) {
        if(close(fd) == -1) {
            std::perror("Ошибка при закрытии канала fifo");
        }
    }
    unlink(path.c_str());
}

void ConnFifo::setup_conn(bool create)
{
    if(create) {
        if(mkfifo(path.c_str(), 0777) == -1) {
            std::perror("Невозможно создать fifo канал");
            return;
        }
    }
    if((fd = ::open(path.c_str(), O_RDWR | O_NONBLOCK)) == -1) {
        std::perror("Не удалось открыть fifo файл");
        return;
    }
    else {
        valid = true;
    }
}

bool ConnFifo::read(std::string& buf, unsigned size)
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
        int read_bytes = ::read(fd, read_buf, size - 1);

        if(read_bytes == -1)
        {
            if(errno == ENXIO)
            {
                std::perror("fifo файл удален");
                return false;
            }
            return false;
        }

        buf.assign(read_buf, read_bytes);
        return true;
    }
}

bool ConnFifo::write(const std::string& buf)
{
    if(!valid) {
        std::perror("Канал недоступен");
        return false;
    }
    else {
        if(::write(fd, buf.c_str(), buf.size()) == -1) {
            std::perror("Ошибка записи в канал");
            return false;
        }
        return true;
    }
}

bool ConnFifo::is_valid() const
{
    return valid;
}
