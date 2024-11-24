#ifndef CONN_SEG_HPP
#define CONN_SEG_HPP

#include <sys/ipc.h>
#include <sys/shm.h>

class ConnSeg {
public:
    ConnSeg(int id, bool create);
    void Read(void *buf, size_t count);
    void Write(const void *buf, size_t count);
    ~ConnSeg();

private:
    int id;
    bool create;
    int shmid;
    void *shmaddr;
};

#endif // CONN_SEG_HPP
