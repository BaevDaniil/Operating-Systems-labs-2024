#pragma once

#include "conn.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string>


class ConnSock : public Conn {
public:
    ConnSock(int hostPort, bool host_flag);
    ~ConnSock();

    static ConnSock* accept(ConnSock& host_listening_conn); // only for host

    bool read(std::string& data, size_t max_size);
    bool write(const std::string& data);

    bool is_valid() const {
        return sock_fd != -1;
    }

private:
    int sock_fd;              // Socket file Descriptor
    sockaddr_in addr;  // socket address

    ConnSock() = default; // for accept
};
