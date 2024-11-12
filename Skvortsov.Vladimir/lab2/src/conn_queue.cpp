// src/conn_queue.cpp
#include "conn_queue.hpp"
#include <iostream>
#include <cstring>
#include <fcntl.h> // For O_CREAT, O_RDWR
#include <sys/stat.h> // For mode constants
#include <unistd.h> // For close

ConnQueue::ConnQueue(const std::string &name, bool create, int max_msg_size, int maxMsgCount)
  : name(name), valid(false), max_msg_size(max_msg_size) {

  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = maxMsgCount;
  attr.mq_msgsize = max_msg_size;
  attr.mq_curmsgs = 0;

  if (create) {
    mq = mq_open(name.c_str(), O_CREAT | O_RDWR, 0644, &attr);
  } else {
    mq = mq_open(name.c_str(), O_RDWR);
  }

  if (mq == (mqd_t)-1) {
    std::cerr << "Error opening message queue: " << strerror(errno) << "\n";
  } else {
    valid = true;
  }
}

ConnQueue::~ConnQueue() {
  close();
}

bool ConnQueue::write(const std::string &message) {
  if (!is_valid()) {
    std::cerr << "Message queue not available for sending\n";
    return false;
  }

  if (mq_send(mq, message.c_str(), message.size(), 0) == -1) {
    std::cerr << "Error sending message: " << strerror(errno) << "\n";
    return false;
  }
  return true;
}

bool ConnQueue::read(std::string& message, size_t max_size) {
  if (!is_valid()) {
    std::cerr << "Message queue not available for receiving\n";
    return false;
  }

  char buffer[max_size];
  memset(buffer, 0, sizeof(buffer));

  ssize_t bytesReceived = mq_receive(mq, buffer, sizeof(buffer), nullptr);
  if (bytesReceived == -1) {
    std::cerr << "Error receiving message: " << strerror(errno) << "\n";
    return false;
  }

  message.assign(buffer, bytesReceived);
  return true;
}

bool ConnQueue::is_valid() const {
  return valid;
}

void ConnQueue::close() {
  if (is_valid()) {
    if (mq_close(mq) == -1) {
      std::cerr << "Error closing message queue: " << strerror(errno) << "\n";
    }
    valid = false;
  }
}
