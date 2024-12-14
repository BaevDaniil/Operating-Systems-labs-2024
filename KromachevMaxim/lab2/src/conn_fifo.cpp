#include "conn_fifo.h"

ConnFifo::ConnFifo(const std::string& path, bool create): path(path)
{
    setup_conn(create);
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

void ConnFifo::setup_conn(bool create)
{
    if(create) {
        if(mkfifo(path.c_str(), 0777) == -1) {
            fprintf(stderr, "Невозможно создать fifo канал.\n");
            return;
        }
    }
    if((fd = ::open(path.c_str(), O_RDWR | O_NONBLOCK)) == -1) {
        fprintf(stderr, "Не удалось открыть fifo файл.\n");
    }
    else {
        valid = true;
    }
}

bool ConnFifo::read(std::string& buf, unsigned size)
{
    if(!valid)
    {
        fprintf(stderr, "Невозможно читать из канала, канал не работает.\n");
        return false;
    }
    else
    {
        char read_buf[size];
        memset(read_buf, '\0', size);
        int read_bytes = ::read(fd, read_buf, size - 1);

        if(read_bytes == -1)
        {
            fprintf(stderr, "Ошибка при чтении данных из канала.\n");
            return false;
        }

        buf.assign(read_buf, read_bytes);
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
