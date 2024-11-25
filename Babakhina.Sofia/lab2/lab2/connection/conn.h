#pragma once

#include <string>

class Conn {
public:
  virtual ~Conn() {}

  virtual bool read(std::string& data, size_t max_size) = 0;
  virtual bool write(const std::string& data) = 0;
  virtual bool is_valid() const = 0;
};
