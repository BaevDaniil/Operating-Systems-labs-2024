#include "conn_fifo.h"

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

std::unique_ptr<Connection> Connection::GetConn(pid_t hostPid, pid_t clientPid, Type type) {
    return std::make_unique<ConnFifo>(hostPid, clientPid, type);
}

ConnFifo::ConnFifo(pid_t hostPid, pid_t clientPid, Type type) : hostPid(hostPid), clientPid(clientPid), type(type) {
    path = "/tmp/fifo" + std::to_string(hostPid)+std::to_string(clientPid);
}

bool ConnFifo::Open() {
    if (type == Type::HOST) {
        if (mkfifo(path.c_str(), 0777) == -1)
            return false;
    }
    fd = open(path.c_str(), O_RDWR);
    return fd != -1;
}

bool ConnFifo::Read(void* buf, size_t count) {
    return read(fd, buf, count) >= 0;
}

bool ConnFifo::Write(void* buf, size_t count) {
    return write(fd, buf, count) >= 0;
}

void ConnFifo::Close() {
    close(fd);
}