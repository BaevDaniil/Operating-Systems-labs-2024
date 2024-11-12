#ifndef CONN_FIFO_H
#define CONN_FIFO_H

#include <string>
#include "conn.hpp"

class ConnFifo : public Conn {
  public:
    ConnFifo(const std::string& path, bool create);
    ~ConnFifo();

    bool write(const std::string& msg) override;
    bool read(std::string& msg, size_t max_size) override;
    bool is_valid() const override;

  private:
    std::string path;
    int fd;
    bool valid;

    void close();
};

#endif
