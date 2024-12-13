#include "conn_shm.hpp"

ShmConnection::ShmConnection(const std::string &id, bool create) : name(id) {
    fd = shm_open(name.c_str(), create ? (O_CREAT | O_RDWR) : O_RDWR, 0666);
    if (fd == -1) {
        throw std::runtime_error("Failed to open shared memory: " + std::string(strerror(errno)));
    }
    ftruncate(fd, max_msg_size);
    addr = mmap(nullptr, max_msg_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        throw std::runtime_error("Failed to map shared memory: " + std::string(strerror(errno)));
    }
}

bool ShmConnection::Read(std::string &message) {
    char buffer[max_msg_size];
    memcpy(buffer, addr, max_msg_size);
    message.assign(buffer);
    return !message.empty();
}

bool ShmConnection::Write(const std::string &message) {
    if (message.size() >= max_msg_size) return false;
    memcpy(addr, message.c_str(), message.size() + 1); // включая \0
    return true;
}

ShmConnection::~ShmConnection() {
    munmap(addr, max_msg_size);
    close(fd);
    shm_unlink(name.c_str());
}
