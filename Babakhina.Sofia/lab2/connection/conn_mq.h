#pragma once

#include "conn.h"


class ConnMq : public Conn {
public:
    ConnMq(key_t key, bool host_flag);
    ~ConnMq();

    bool write(const std::string& msg) override;
    bool read(std::string& msg, size_t max_size) override;
    bool is_valid() const override;

    void close();

private:
    int queue_id;
    bool is_host; 
};

