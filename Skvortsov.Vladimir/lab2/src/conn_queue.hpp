#ifndef CONN_QUEUE_H
#define CONN_QUEUE_H

#include <string>
#include <mqueue.h>
#include "conn.hpp"

class ConnQueue : Conn {
  public:
    ConnQueue(const std::string &name, bool create, int max_msg_size = 1024, int max_msg_count = 10);
    ~ConnQueue();

    bool write(const std::string& msg);
    bool read(std::string& msg);
    bool is_valid() const;

  private:
    mqd_t mq;
    std::string name;
    bool valid;
    int max_msg_size;

    void close();
};

#endif
