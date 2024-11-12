#include "conn_fifo.hpp"
#include <iostream>
#include <fcntl.h>  // For O_WRONLY, O_RDONLY
#include <sys/stat.h> // For mkfifo
#include <unistd.h> // For close, read, write
#include <cstring> // For strerror

ConnFifo::ConnFifo(const std::string& path, bool create) : path(path), fd(-1), valid(false) {
  if (create) {
    // Create new FIFO
    if (mkfifo(path.c_str(), 0644) == -1) {
      if (errno != EEXIST) {
        std::cerr << "Ошибка создания FIFO: " << strerror(errno) << "\n";
        return;
      }
    }
  }

  fd = open(path.c_str(), create ? O_RDWR : O_RDONLY);
  if (fd == -1) {
    std::cerr << "Error opening FIFO: " << strerror(errno) << "\n";
  } else {
    valid = true;
  }
}

ConnFifo::~ConnFifo() {
  close();
  if (valid) {
    // Delete FIFO
    if (unlink(path.c_str()) == -1) {
      std::cerr << "Error removing FIFO: " << strerror(errno) << "\n";
    }
  }
}

bool ConnFifo::write(const std::string &msg) {
  if (!is_valid()) {
    std::cerr << "FIFO not available for writing\n";
    return false;
  }

  ssize_t bytesWritten = ::write(fd, msg.c_str(), msg.size());
  if (bytesWritten == -1) {
    std::cerr << "Error writing to FIFO: " << strerror(errno) << "\n";
    return false;
  }
  return true;
}

bool ConnFifo::read(std::string &data, size_t max_size) {
  if (!is_valid()) {
    std::cerr << "FIFO not available for reading\n";
    return false;
  }

  char buffer[max_size];
  memset(buffer, 0, sizeof(buffer));

  ssize_t bytesRead = ::read(fd, buffer, sizeof(buffer));
  if (bytesRead == -1) {
    std::cerr << "Error reading from FIFO: " << strerror(errno) << "\n";
    return false;
  }

  data.assign(buffer, bytesRead);
  return true;
}

bool ConnFifo::is_valid() const {
  return valid;
}

void ConnFifo::close() {
  if (is_valid() && fd != -1) {
    if (::close(fd) == -1) {
      std::cerr << "Error closing FIFO: " << strerror(errno) << "\n";
    }
    fd = -1;
  }
}
