#include "conn_socket.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

ConnSocket::ConnSocket() : socket_fd(-1) {
  memset(&addr, 0, sizeof(addr));
};

ConnSocket::ConnSocket(int socket_fd, sockaddr_in addr) : socket_fd(socket_fd), addr(addr) {};

ConnSocket::~ConnSocket() {
  close();
};

bool ConnSocket::create_server_socket(int port) {
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    std::cerr << "Error creating socket\n";
    return false;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    std::cerr << "Error binding socket\n";
    close();
    return false;
  }

  if (listen(socket_fd, 5) < 0) {
    std::cerr << "Error listening on socket\n";
    close();
    return false;
  }

  return true;
};

ConnSocket ConnSocket::accept_connection() {
  sockaddr_in client_addr;
  socklen_t clientLen = sizeof(client_addr);
  int client_socket_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &clientLen);
  if (client_socket_fd < 0) {
    std::cerr << "Error accepting connection\n";
    return ConnSocket();
  }
  return ConnSocket(client_socket_fd, client_addr);
}

bool ConnSocket::connect_to_server(const std::string& address, int port) {
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_fd < 0) {
    std::cerr << "Error creating socket\n";
    return false;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) <= 0) {
    std::cerr << "Invalid address\n";
    close();
    return false;
  }

  if (connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    std::cerr << "Error connecting to server\n";
    close();
    return false;
  }

  return true;
};

bool ConnSocket::write(const std::string& msg) {
  if (!is_valid()) {
    std::cerr << "Socket not available for writing\n";
    return false;
  }

  ssize_t bytes_written = send(socket_fd, msg.c_str(), msg.size(), 0);
  if (bytes_written < 0) {
    std::cerr << "Error sending msg\n";
    return false;
  }
  return true;
};

bool ConnSocket::read(std::string& msg, size_t max_size) {
  if (!is_valid()) {
    std::cerr << "Socket not available for reading\n";
    return false;
  }

  char buffer[max_size];
  memset(buffer, '\0', max_size);

  ssize_t bytes_read = recv(socket_fd, buffer, max_size - 1, 0);
  if (bytes_read == 0) {
    return false;
  }
  if (bytes_read == -1) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      std::cerr << "No msg available to read, try again later\n";
    } else if (errno == EINTR) {
      std::cerr << "Read interrupted by a signal, retrying\n";
      return read(msg, max_size); // Retry reading
    } else {
      std::cerr << "Error receiving msg: " << strerror(errno) << " (errno: " << errno << ")\n";
    }
    return false;
  }

  msg.assign(buffer, bytes_read);
  return true;
};

bool ConnSocket::is_valid() const {
  return socket_fd >= 0;
};

void ConnSocket::close() {
  if (is_valid()) {
    ::close(socket_fd);
    socket_fd = -1;
  }
};
