#include "conn_sock.h"


ConnSock::ConnSock(int host_port, bool host_flag) : sock_fd(-1) {
    std::cout << "Creating socket for Host" << std::endl;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        std::cout << "Socket creation failed" << std::endl;
        return;
    }

    // Setting the socket to non-blocking mode
    int flags = fcntl(sock_fd, F_GETFL, 0);
    fcntl(sock_fd, F_SETFL, flags | O_NONBLOCK);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(host_port); 

    if (host_flag) {
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
            std::cout << "Bind failed" << std::endl;
            ::close(sock_fd);
            sock_fd = -1;
            return;
        }

        if (listen(sock_fd, 1) == -1) {
            std::cout << "Listen failed" << std::endl;
            ::close(sock_fd);
            sock_fd = -1;
            return;
        }

        std::cout << "Socket successfully created and listening" << std::endl;
    }
    else {
        if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
            std::cout << "Invalid host IP address" << std::endl;
            ::close(sock_fd);
            sock_fd = -1;
            return;
        }

        int result = connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr));
        if (result == -1) {
            if (errno == EINPROGRESS) {
                std::cout << "Connection is in progress..." << std::endl;

                // Using select to wait for connection termination
                fd_set write_fds;
                FD_ZERO(&write_fds);
                FD_SET(sock_fd, &write_fds);

                struct timeval timeout;
                timeout.tv_sec = 5;  
                timeout.tv_usec = 0;

                int select_result = select(sock_fd + 1, NULL, &write_fds, NULL, &timeout);

                if (select_result > 0) {
                    int so_error;
                    socklen_t len = sizeof(so_error);
                    if (getsockopt(sock_fd, SOL_SOCKET, SO_ERROR, &so_error, &len) < 0) {
                        std::cout << "Getsockopt failed: " << strerror(errno) << std::endl;
                        ::close(sock_fd);
                        sock_fd = -1;
                        return;
                    }

                    if (so_error != 0) {
                        std::cout << "Connect failed with error: " << strerror(so_error) << std::endl;
                        ::close(sock_fd);
                        sock_fd = -1;
                        return;
                    } else {
                        std::cout << "Connected to host successfully" << std::endl;
                    }
                } else {
                    std::cout << "Connection timed out or failed: " << strerror(errno) << std::endl;
                    ::close(sock_fd);
                    sock_fd = -1;
                    return;
                }
            } else {
                std::cout << "Connect failed: " << strerror(errno) << std::endl;
                ::close(sock_fd);
                sock_fd = -1;
                return;
            }
        }
        std::cout << "Connected to host successfully" << std::endl;
    }
}


ConnSock* ConnSock::accept(ConnSock& host_listening_conn) {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = ::accept(host_listening_conn.sock_fd, (struct sockaddr*)&client_addr, &client_len);

    if (client_fd == -1) {
        // std::cout << "Accept failed" << std::endl;
        return nullptr;
    }

    // Setting the socket to non-blocking mode
    int flags = fcntl(client_fd, F_GETFL, 0);
    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

    std::cout << "Client connection accepted" << std::endl;

    // Make new socket for communication with client
    ConnSock* conn = new ConnSock();
    conn->sock_fd = client_fd;
    conn->addr = client_addr;
    return conn;
}


bool ConnSock::read(std::string& data, size_t max_size) {
    if (is_valid()) {
        char buf[1024];
        ssize_t bytes_read = recv(sock_fd, buf, max_size, 0);
        if (bytes_read <= 0) {
            return false;
        }
        data = buf;
        data.resize(bytes_read);
        return true;
    }
    else {
        return false;
    }
}


bool ConnSock::write(const std::string& data) {
    if (!is_valid()) {
        std::cerr << "Invalid socket." << std::endl;
        return false;
    }

    if (data.empty()) {
        std::cerr << "Attempt to write empty data." << std::endl;
        return false;
    }

    ssize_t bytes_sent = send(sock_fd, data.c_str(), data.size(), 0);

    if (bytes_sent == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Socket is not ready for writing, we return false
            return false;
        } else {
            // Other errors
            std::cerr << "Send failed: " << strerror(errno) << std::endl;
            return false;
        }
    }

    // Partial sending of data
    if (bytes_sent < static_cast<ssize_t>(data.size())) {
        std::cerr << "Warning: Partial write occurred (" << bytes_sent << " of " << data.size() << " bytes)." << std::endl;
        return false;
    }

    return true;
}


ConnSock::~ConnSock() {
    close();
}


bool ConnSock::is_valid() const {
    return sock_fd != -1;
}


void ConnSock::close() {
    if (is_valid()) {
        shutdown(sock_fd, SHUT_RD);
        if (::close(sock_fd) == -1) {
            std::cerr << "Sock closing error: " << strerror(errno) << "\n";
        }
        sock_fd = -1;
    }
}
