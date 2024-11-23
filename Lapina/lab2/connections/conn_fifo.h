#ifndef CONN_FIFO_H
#define CONN_FIFO_H

#include "connection.h"

#include <string>

class ConnFifo : public Connection {
private:
    pid_t hostPid;
    Type type;
    int fd;
    std::string path;
public:
    ConnFifo(pid_t hostPid, Type type);
    virtual bool Open() override;
    virtual bool Read(void* buf, size_t count) override;
    virtual bool Write(void* buf, size_t count) override;
    virtual void Close() override;
    ~ConnFifo() = default;
};

#endif