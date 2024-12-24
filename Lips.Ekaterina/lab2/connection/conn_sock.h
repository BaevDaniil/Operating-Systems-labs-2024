#pragma once

#include "conn.h"

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>
#include <fcntl.h>  
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>


class ConnSock : public Conn {
public:
    ConnSock(int hostPort, bool host_flag);
    ~ConnSock();

    static ConnSock* accept(ConnSock& host_listening_conn); // only for host

    bool read(std::string& data, size_t max_size) override;
    bool write(const std::string& data) override;

    void close();

    bool is_valid() const override;

private:
    int sock_fd;              
    sockaddr_in addr;  

    ConnSock() = default; // for accept
};
