#include "conn_socket.hpp"
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/socket.h>

ConnSocket::ConnSocket() : sockfd(-1) {
  memset(&addr, 0, sizeof(addr));
}

ConnSocket::ConnSocket(int sockfd, sockaddr_in addr) : sockfd(sockfd), addr(addr) {}

ConnSocket::~ConnSocket() {
  Close();
}

bool ConnSocket::create_server_socket(int port) {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Ошибка создания сокета\n";
    return false;
  }

  int opt = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    std::cerr << "Ошибка установки опций сокета\n";
    Close();
    return false;
  }

  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    std::cerr << "Ошибка привязки сокета\n";
    Close();
    return false;
  }

  if (listen(sockfd, SOMAXCONN) < 0) {
    std::cerr << "Ошибка прослушивания сокета\n";
    Close();
    return false;
  }

  return true;
}

ConnSocket ConnSocket::AcceptConnection() {
  sockaddr_in clientAddr;
  socklen_t clientLen = sizeof(clientAddr);
  int clientSockfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
  if (clientSockfd < 0) {
    std::cerr << "Ошибка принятия соединения\n";
    return ConnSocket();
  }
  return ConnSocket(clientSockfd, clientAddr);
}

bool ConnSocket::ConnectToServer(const std::string& address, int port) {
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    std::cerr << "Ошибка создания сокета\n";
    return false;
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  if (inet_pton(AF_INET, address.c_str(), &addr.sin_addr) <= 0) {
    std::cerr << "Неверный адрес\n";
    Close();
    return false;
  }

  if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    std::cerr << "Ошибка подключения к серверу\n";
    Close();
    return false;
  }

  return true;
}

bool ConnSocket::write(const void* buffer, size_t size) {
  ssize_t bytesSent = send(sockfd, buffer, size, 0);
  if (bytesSent < 0) {
    std::cerr << "Ошибка отправки данных\n";
    return false;
  }
  return true;
}

ssize_t ConnSocket::read(void* buffer, size_t size) {
    ssize_t bytesRead = recv(sockfd, buffer, size, 0);
    if (bytesRead < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Non-blocking mode: no data available right now
            std::cerr << "No data available to read, try again later\n";
        } else if (errno == EINTR) {
            // Interrupted by a signal: retry the operation
            std::cerr << "Read interrupted by a signal, retrying\n";
            return read(buffer, size); // Retry reading
        } else {
            // Other errors
            std::cerr << "Ошибка получения данных: " << strerror(errno) << " (errno: " << errno << ")\n";
        }
    }
    return bytesRead;
}

bool ConnSocket::is_valid() const {
  return sockfd >= 0;
}

void ConnSocket::Close() {
  if (is_valid()) {
    close(sockfd);
    sockfd = -1;
  }
}
