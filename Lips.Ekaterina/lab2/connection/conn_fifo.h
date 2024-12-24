#pragma once

#include "conn.h"

#include <string>
#include <iostream>
#include <fcntl.h>  
#include <sys/stat.h> 
#include <unistd.h> 
#include <cstring> 


class ConnFifo : public Conn {
public:
    ConnFifo(const std::string& path, bool create);
    ~ConnFifo();

    bool write(const std::string& msg) override;
    bool read(std::string& msg, size_t max_size) override;
    bool is_valid() const override;

    void close();

private:
    std::string path;
    int fd;
    bool valid;
};