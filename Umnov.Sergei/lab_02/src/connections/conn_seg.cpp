#include "conn_seg.hpp"
#include <cstring>
#include <iostream>
#include <cerrno>

ConnSeg::ConnSeg(int id, bool create, size_t size) : id(id), create(create), size(size) {
    if (create) {
        shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
        if (shmid == -1) {
            throw std::runtime_error("Failed to create shared memory: " + std::string(strerror(errno)));
        }
    } else {
        shmid = shmget(id, size, 0666);
        if (shmid == -1) {
            throw std::runtime_error("Failed to access shared memory: " + std::string(strerror(errno)));
        }
    }

    shmaddr = shmat(shmid, nullptr, 0);
    if (shmaddr == (void *)-1) {
        throw std::runtime_error("Failed to attach shared memory: " + std::string(strerror(errno)));
    }
}

ConnSeg::~ConnSeg() {
    if (shmdt(shmaddr) == -1) {
        std::cerr << "Failed to detach shared memory: " << strerror(errno) << std::endl;
    }

    if (create) {
        if (shmctl(shmid, IPC_RMID, nullptr) == -1) {
            std::cerr << "Failed to remove shared memory: " << strerror(errno) << std::endl;
        }
    }
}

void ConnSeg::Read(void *buf, size_t count) {
    if (count > size) {
        throw std::out_of_range("Read size exceeds shared memory segment size");
    }
    memcpy(buf, shmaddr, count);
}

void ConnSeg::Write(const void *buf, size_t count) {
    if (count > size) {
        throw std::out_of_range("Write size exceeds shared memory segment size");
    }
    memcpy(shmaddr, buf, count);
}
