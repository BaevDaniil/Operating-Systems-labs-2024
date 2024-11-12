#include "conn_queue.hpp"
#include <iostream>
#include <cstring>
#include <fcntl.h>    // Для O_CREAT, O_RDWR
#include <sys/stat.h>   // Для mode constants
#include <unistd.h>   // Для close

ConnQueue::ConnQueue(const std::string& name, bool create, int max_msg_size, int maxMsgCount)
  : name(name), valid(false), max_msg_size(max_msg_size) {

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = maxMsgCount;
  attr.mq_msgsize = max_msg_size;
  attr.mq_curmsgs = 0;

  if (create) {
    // Создание новой очереди сообщений
    mq = mq_open(name.c_str(), O_CREAT | O_RDWR, 0644, &attr);
  } else {
    // Открытие существующей очереди сообщений
    mq = mq_open(name.c_str(), O_RDWR);
  }

  if (mq == (mqd_t)-1) {
    std::cerr << "Ошибка открытия очереди сообщений: " << strerror(errno) << "\n";
  } else {
    valid = true;
  }
}

ConnQueue::~ConnQueue() {
  Close();
}

bool ConnQueue::Send(const std::string& message) {
  if (!is_valid()) {
    std::cerr << "Очередь сообщений недоступна для отправки\n";
    return false;
  }

  if (mq_send(mq, message.c_str(), message.size(), 0) == -1) {
    std::cerr << "Ошибка отправки сообщения: " << strerror(errno) << "\n";
    return false;
  }
  return true;
}

bool ConnQueue::Receive(std::string& message) {
  if (!is_valid()) {
    std::cerr << "Очередь сообщений недоступна для получения\n";
    return false;
  }

  char buffer[max_msg_size];
  memset(buffer, 0, sizeof(buffer));

  ssize_t bytesReceived = mq_receive(mq, buffer, sizeof(buffer), nullptr);
  if (bytesReceived == -1) {
    std::cerr << "Ошибка получения сообщения: " << strerror(errno) << "\n";
    return false;
  }

  message.assign(buffer, bytesReceived);
  return true;
}

bool ConnQueue::is_valid() const {
  return valid;
}

void ConnQueue::Close() {
  if (is_valid()) {
    if (mq_close(mq) == -1) {
      std::cerr << "Ошибка закрытия очереди сообщений: " << strerror(errno) << "\n";
    }
    valid = false;
  }
}