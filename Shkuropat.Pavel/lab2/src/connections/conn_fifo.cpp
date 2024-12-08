#include "conn_fifo.hpp"
#include <cstring>
#include <iostream>

ConnFifo::ConnFifo(int id, bool create) : id(id), create(create) {
    // Генерация уникального пути
    fifoPath = "/tmp/myfifo_" + std::to_string(id);

    if (create) {
        // Создание FIFO
        if (mkfifo(fifoPath.c_str(), 0666) == -1 && errno != EEXIST) {
            throw std::runtime_error("Failed to create FIFO: " + std::string(strerror(errno)));
        }
    }

    // Открытие FIFO
    fifo = open(fifoPath.c_str(), O_RDWR);
    if (fifo == -1) {
        throw std::runtime_error("Failed to open FIFO: " + std::string(strerror(errno)));
    }
}

void ConnFifo::Read(char *buf, size_t count) {
    ssize_t bytesRead = read(fifo, buf, count);
    if (bytesRead == -1) {
        throw std::runtime_error("Read error: " + std::string(strerror(errno)));
    }
    buf[bytesRead] = '\0'; // Завершающий нулевой символ
}

void ConnFifo::Write(const char *buf, size_t count) {
    ssize_t bytesWritten = write(fifo, buf, count);
    if (bytesWritten == -1) {
        throw std::runtime_error("Write error: " + std::string(strerror(errno)));
    }
}

ConnFifo::~ConnFifo() {
    if (fifo != -1) {
        close(fifo);
    }

    // Удаление FIFO только если объект его создавал
    if (create) {
        if (unlink(fifoPath.c_str()) == -1) {
            std::cerr << "Failed to unlink FIFO: " << strerror(errno) << std::endl;
        }
    }
}
