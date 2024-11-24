#include "conn_mq.hpp"
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

ConnMq::ConnMq(int id, bool create) : id(id), create(create) {
    if (create) {
        mq = mq_open("/myqueue", O_CREAT | O_RDWR, 0644, nullptr);
    } else {
        mq = mq_open("/myqueue", O_RDWR);
    }
}

void ConnMq::Read(char *buf, size_t count) {
    mq_receive(mq, buf, count, nullptr);
}

void ConnMq::Write(const char *buf, size_t count) {
    mq_send(mq, buf, count, 0);
}

ConnMq::~ConnMq() {
    mq_close(mq);
    if (create) {
        mq_unlink("/myqueue");
    }
}
