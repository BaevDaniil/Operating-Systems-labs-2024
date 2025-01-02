#pragma once

#include <string>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/msg.h>

class Conn {
public:
  virtual ~Conn() {}

  virtual bool read(std::string& data, size_t max_size) = 0;
  virtual bool write(const std::string& data) = 0;
  virtual bool is_valid() const = 0;

  virtual void close() = 0;
};
