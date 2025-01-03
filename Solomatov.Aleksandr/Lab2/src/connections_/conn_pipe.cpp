#include "conn_pipe.hpp"

PipeConnection::PipeConnection(const std::string &id, bool create) : name(id) {
    if (create) {
        if (mkfifo(name.c_str(), 0666) < 0 && errno != EEXIST) {
            throw std::runtime_error("Failed to create pipe: " + std::string(strerror(errno)));
        }
    }
    fd = open(name.c_str(), O_RDWR);
    if (fd < 0) {
        throw std::runtime_error("Failed to open pipe: " + std::string(strerror(errno)));
    }
}

bool PipeConnection::Read(std::string &message) {
    constexpr size_t BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
    if (bytesRead > 0) {
        message.assign(buffer, bytesRead);
        return true;
    } else if (bytesRead == 0) {
        // EOF detected
        return false;
    } else {
        // Handle errors if needed
        return false;
    }
}

bool PipeConnection::Write(const std::string &message) {
    ssize_t bytesWritten = write(fd, message.c_str(), message.size());
    if (bytesWritten == -1) {
        // Optional: log or handle write errors
        return false;
    }
    return bytesWritten == static_cast<ssize_t>(message.size());
}

PipeConnection::~PipeConnection() {
    close(fd);
    // Optional: Comment this out if unlink should not be automatic
    unlink(name.c_str());
}
