#pragma once
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "connection.hpp"

struct msgbuf
{
    long mtype;
    char mtext[1];
};

class MessageQueue final : public Connection<key_t>
{
    MessageQueue(key_t id, bool create);
    virtual bool Read(void *buf, size_t count) override;
    virtual bool Write(void *buf, size_t count) override;
    virtual ~MessageQueue() override;
private:
    int msgid;
};