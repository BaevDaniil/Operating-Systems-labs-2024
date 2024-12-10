#ifndef CONN_SEG_HPP
#define CONN_SEG_HPP

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdexcept>
#include <cstddef>

class ConnSeg {
public:
    ConnSeg(int id, bool create, size_t size = 1024);
    ~ConnSeg();
    void Read(void *buf, size_t count);
    void Write(const void *buf, size_t count);

private:
    int id;
    bool create;
    int shmid;
    size_t size;
    void *shmaddr;
};

#endif // CONN_SEG_HPP
