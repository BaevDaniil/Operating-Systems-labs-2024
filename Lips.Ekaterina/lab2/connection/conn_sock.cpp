#include "conn_sock.h"

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <arpa/inet.h>

ConnSock::ConnSock(int hostPort, bool host_flag) : sock_fd(-1) {
    std::cout << "Creating socket for Host" << std::endl;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        std::cout << "Socket creation failed" << std::endl;
        return;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(hostPort); // Порт

    if (host_flag) {
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            std::cout << "Bind failed" << std::endl;
            close(sock_fd);
            sock_fd = -1;
            return;
        }

        if (listen(sock_fd, 1) == -1) {
            std::cout << "Listen failed" << std::endl;
            close(sock_fd);
            sock_fd = -1;
            return;
        }

        std::cout << "Socket successfully created and listening" << std::endl;
    }
    else {
        if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
            std::cout << "Invalid host IP address" << std::endl;
            close(sock_fd);
            sock_fd = -1;
            return;
        }

        if (connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            std::cout << "Connect failed" << std::endl;
            close(sock_fd);
            sock_fd = -1;
            return;
        }
        std::cout << "Connected to host successfully" << std::endl;
    }
}

ConnSock* ConnSock::accept(ConnSock& host_listening_conn) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = ::accept(host_listening_conn.sock_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        std::cout << "Accept failed" << std::endl;
        return nullptr;
    }

    std::cout << "Client connection accepted" << std::endl;
    // make new socket for communication with client
    ConnSock* conn = new ConnSock();
    conn->sock_fd = client_fd;
    conn->addr = client_addr;
    return conn;
}

bool ConnSock::read(std::string& data, size_t max_size) {
    char buf[1024];
    ssize_t bytes_read = recv(sock_fd, buf, max_size, 0);
    if (bytes_read <= 0) {
        return false;
    }
    data = buf;
    data.resize(bytes_read);
    return true;
}

bool ConnSock::write(const std::string& data) {
    ssize_t bytes_sent = send(sock_fd, data.c_str(), data.size(), 0);
    if (bytes_sent <= 0) {
        return false;
    }
    return true;
}

ConnSock::~ConnSock() {
    if (sock_fd != -1) {
        close(sock_fd);
    }
}
