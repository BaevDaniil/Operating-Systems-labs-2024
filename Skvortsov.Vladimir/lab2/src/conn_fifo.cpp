#include "conn_fifo.hpp"
#include <iostream>
#include <fcntl.h>  // Для O_WRONLY, O_RDONLY
#include <sys/stat.h> // Для mkfifo
#include <unistd.h> // Для close, read, write
#include <cstring> // Для strerror

ConnFifo::ConnFifo(const std::string& path, bool create) : path(path), fd(-1), valid(false) {
  if (create) {
    // Создание нового FIFO
    if (mkfifo(path.c_str(), 0644) == -1) {
      if (errno != EEXIST) {
        std::cerr << "Ошибка создания FIFO: " << strerror(errno) << "\n";
        return;
      }
    }
  }

  // Открытие FIFO
  fd = open(path.c_str(), create ? O_RDWR : O_RDONLY);
  if (fd == -1) {
    std::cerr << "Ошибка открытия FIFO: " << strerror(errno) << "\n";
  } else {
    valid = true;
  }
}

ConnFifo::~ConnFifo() {
  close();
  if (valid) {
    // Удаление FIFO
    if (unlink(path.c_str()) == -1) {
      std::cerr << "Ошибка удаления FIFO: " << strerror(errno) << "\n";
    }
  }
}

bool ConnFifo::write(const std::string& msg) {
  if (!is_valid()) {
    std::cerr << "FIFO недоступен для записи\n";
    return false;
  }

  ssize_t bytesWritten = write(fd, msg.c_str(), msg.size());
  if (bytesWritten == -1) {
    std::cerr << "Ошибка записи в FIFO: " << strerror(errno) << "\n";
    return false;
  }
  return true;
}

bool ConnFifo::read(std::string& data, size_t max_size) {
  if (!is_valid()) {
    std::cerr << "FIFO недоступен для чтения\n";
    return false;
  }

  char buffer[max_size];
  memset(buffer, 0, sizeof(buffer));

  ssize_t bytesRead = read(fd, buffer, sizeof(buffer));
  if (bytesRead == -1) {
    std::cerr << "Ошибка чтения из FIFO: " << strerror(errno) << "\n";
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
    if (close(fd) == -1) {
      std::cerr << "Ошибка закрытия FIFO: " << strerror(errno) << "\n";
    }
    fd = -1;
  }
}
