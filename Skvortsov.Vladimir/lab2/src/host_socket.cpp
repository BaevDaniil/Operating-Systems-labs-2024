#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include "conn_socket.hpp"

const int PORT = 12345;
std::vector<ConnSocket> clients;
std::mutex clients_mutex;

void handleClient(ConnSocket clientSocket) {
  std::string message;
  while (true) {
    if (!clientSocket.read(message, 1024)) {
      std::cerr << "Error receiving message\n";
      break;
    }
    std::cout << "Received message: " << message << std::endl;
  }

  clientSocket.close();
  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
  }
}

int main() {
  ConnSocket serverSocket;

  if (!serverSocket.create_server_socket(PORT)) {
    std::cerr << "Error creating server socket\n";
    return 1;
  }

  while (true) {
    ConnSocket clientSocket = serverSocket.accept_connection();
    if (!clientSocket.is_valid()) {
      std::cerr << "Error accepting connection\n";
      continue;
    }

    {
      std::lock_guard<std::mutex> lock(clients_mutex);
      clients.push_back(clientSocket);
    }

    std::thread(handleClient, std::move(clientSocket)).detach();
  }

  return 0;
}
