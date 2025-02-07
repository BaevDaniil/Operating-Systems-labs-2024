#include "conn_mq.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>

ConnMq::ConnMq(int id, bool create) : id(id), create(create) {
    // Генерация уникального имени очереди
    queueName = "/myqueue_" + std::to_string(id);

    if (create) {
        // Создание очереди
        struct mq_attr attr = {};
        attr.mq_flags = 0;           // Блокирующий режим
        attr.mq_maxmsg = 10;         // Максимальное количество сообщений
        attr.mq_msgsize = 1024;      // Максимальный размер сообщения

        mq = mq_open(queueName.c_str(), O_CREAT | O_RDWR, 0644, &attr);
        if (mq == (mqd_t)-1) {
            throw std::runtime_error("Failed to create message queue: " + std::string(strerror(errno)));
        }
    } else {
        // Открытие существующей очереди
        mq = mq_open(queueName.c_str(), O_RDWR);
        if (mq == (mqd_t)-1) {
            throw std::runtime_error("Failed to open message queue: " + std::string(strerror(errno)));
        }
    }
}

void ConnMq::Read(char *buf, size_t count) {
    ssize_t bytesRead = mq_receive(mq, buf, count, nullptr);
    if (bytesRead == -1) {
        throw std::runtime_error("Read error: " + std::string(strerror(errno)));
    }
    buf[bytesRead] = '\0'; // Завершающий символ
}

void ConnMq::Write(const char *buf, size_t count) {
    if (mq_send(mq, buf, count, 0) == -1) {
        throw std::runtime_error("Write error: " + std::string(strerror(errno)));
    }
}

ConnMq::~ConnMq() {
    if (mq_close(mq) == -1) {
        std::cerr << "Failed to close message queue: " << strerror(errno) << std::endl;
    }

    if (create) {
        if (mq_unlink(queueName.c_str()) == -1) {
            std::cerr << "Failed to unlink message queue: " << strerror(errno) << std::endl;
        }
    }
}
