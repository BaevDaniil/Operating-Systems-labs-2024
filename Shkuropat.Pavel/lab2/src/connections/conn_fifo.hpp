#ifndef CONN_FIFO_HPP
#define CONN_FIFO_HPP

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <stdexcept>

class ConnFifo {
public:
    ConnFifo(int id, bool create);
    void Read(char *buf, size_t count);
    void Write(const char *buf, size_t count);
    ~ConnFifo();

private:
    int id;
    bool create;
    int fifo;
    std::string fifoPath;
};

#endif // CONN_FIFO_HPP
