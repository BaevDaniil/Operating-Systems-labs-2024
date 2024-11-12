#ifndef CONN_SOCKET_H
#define CONN_SOCKET_H

#include <string>
#include <netinet/in.h>
#include "conn.hpp"

class ConnSocket : public Conn {
  public:
    ConnSocket();
    ~ConnSocket();

    bool create_server_socket(int port);
    ConnSocket accept_connection();
    bool connect_to_server(const std::string &address, int port);

    bool write(const std::string& data) override;
    bool read(std::string& data, size_t max_size) override;
    bool is_valid() const override;

    bool operator != (const ConnSocket &other) const {
        return this->sockfd != other.sockfd;
    };
    bool operator == (const ConnSocket &other) const {
        return this->sockfd == other.sockfd;
    };

  private:
    int sockfd;
    sockaddr_in addr;

    ConnSocket(int sockfd, sockaddr_in addr);

    void close();
};

#endif
