#ifndef CONN_MQ_H
#define CONN_MQ_H

#include "connection.h"

#include <string>
#include <mqueue.h>

class ConnMq : public Connection {
private:
    pid_t hostPid;
    Type type;
    std::string path;
    mqd_t mqd;
    static constexpr int msgSize = 64;
public:
    
    ConnMq(pid_t hostPid, Type type);
    
    virtual bool Open() override;
    virtual bool Read(void* buf, size_t count) override;
    virtual bool Write(void* buf, size_t count) override;
    virtual void Close() override;
    
    ~ConnMq() = default;
};

#endif