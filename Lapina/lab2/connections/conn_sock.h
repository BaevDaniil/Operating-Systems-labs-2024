#ifndef CONN_SOCK_H
#define CONN_SOCK_H

#include "connection.h"

#include <sys/socket.h>
#include <sys/un.h>

class ConnSock : public Connection {
private:
    int sockfd;
    pid_t hostPid;
    pid_t clientPid;
    Type type;
    struct sockaddr_un addr;
    socklen_t addrLen;

public:
    ConnSock(pid_t hostPid, pid_t clientPid, Type type);
    
    virtual bool Open() override;
    virtual bool Read(void* buf, size_t count) override;
    virtual bool Write(void* buf, size_t count) override;
    virtual void Close() override;
    
    ~ConnSock() = default;
};
#endif