#include "conn_fifo.h"

ConnFifo::ConnFifo(const std::string& path, bool create): path(path)
{
    if(create) {
        if(mkfifo(path.c_str(), O_RDWR) == -1) {
            fprintf(stderr, "Невозможно создать fifo.\n");
            return;
        }
    }
    if((fd = ::open(path.c_str(), O_RDWR)) == -1) {
        fprintf(stderr, "Невозможно создать fifo.\n");
    }
    else {
        valid = true;
    }
}

ConnFifo::~ConnFifo()
{
    if(valid) {
        if(close(fd) == -1) {
            fprintf(stderr, "Ошибка при закрытии канала fifo.\n");
        }
    }
    unlink(path.c_str());
}

bool ConnFifo::read(const std::string& buf, unsigned size)
{
    if(!valid) {
        fprintf(stderr, "Невозможно читать из канала, канал не работает.\n");
        return false;
    }
    else {
        char read_buf[size + 1];
        memset(read_buf, 0x00, size + 1);
        int read_bytes = ::read(fd, read_buf, sizeof(size));
        if(read_bytes == -1) {
            fprintf(stderr, "Ошибка при чтении данных из канала.\n");
            return false;
        }
        return true;
    }
}

bool ConnFifo::write(const std::string& buf)
{
    if(!valid) {
        fprintf(stderr, "Невозможно писать в канал, канал не работает.\n");
        return false;
    }
    else {
        if(::write(fd, buf.c_str(), sizeof(buf)) == -1) {
            fprintf(stderr, "Не удалось записать данные в канал.\n");
            return false;
        }
        return true;
    }
}

bool ConnFifo::is_valid() const
{
    return valid;
}
