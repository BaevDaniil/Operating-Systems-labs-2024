#include "conn_pipe.hpp"

PipeConnection::PipeConnection(const std::string &id, bool create) : name(id) {
    if (create) {
        // Создание именованного pipe
        mkfifo(name.c_str(), 0666); 
    }
    fd = open(name.c_str(), create ? O_RDWR : O_RDONLY);
    if (fd < 0) {
        throw std::runtime_error("Failed to open pipe: " + std::string(strerror(errno)));
    }
}

bool PipeConnection::Read(std::string &message) {
    char buffer[1024] = {0};
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead > 0) {
        message.assign(buffer, bytesRead);
        return true;
    }
    return false;
}

bool PipeConnection::Write(const std::string &message) {
    ssize_t bytesWritten = write(fd, message.c_str(), message.size());
    return bytesWritten == static_cast<ssize_t>(message.size());
}

PipeConnection::~PipeConnection() {
    close(fd);
    unlink(name.c_str());
}
