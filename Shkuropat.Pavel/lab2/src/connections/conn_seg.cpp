#include "conn_seg.hpp"
#include <iostream>
#include <string.h>

ConnSeg::ConnSeg(int id, bool create) : id(id), create(create) {
    if (create) {
        shmid = shmget(IPC_PRIVATE, 1024, IPC_CREAT | 0666);
        shmaddr = shmat(shmid, nullptr, 0);
    } else {
        shmid = shmget(id, 1024, 0666);
        shmaddr = shmat(shmid, nullptr, 0);
    }
}

void ConnSeg::Read(void *buf, size_t count) {
    memcpy(buf, shmaddr, count);
}

void ConnSeg::Write(const void *buf, size_t count) {
    memcpy(shmaddr, buf, count);
}

ConnSeg::~ConnSeg() {
    shmdt(shmaddr);
    if (create) {
        shmctl(shmid, IPC_RMID, nullptr);
    }
}
