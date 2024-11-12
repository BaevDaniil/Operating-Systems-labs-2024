#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cstring>
#include "conn_socket.hpp"

const int PORT = 12345;

// Vector to keep track of all client sockets
std::vector<ConnSocket> clients;
std::mutex clients_mutex;

// Function to broadcast a message to all clients except the sender
void broadcastMessage(const std::string &message, ConnSocket &sender) {
  std::lock_guard<std::mutex> lock(clients_mutex);
  std::cout << "Broadcasting message: " << message << std::endl;
  for (auto &client : clients) {
    if (client.is_valid() && client != sender) {
      if (client.write(message)) {
        std::cout << "Sent message to client with fd: " << client.sockfd << std::endl;
      } else {
        std::cerr << "Failed to send message to client with fd: " << client.sockfd << std::endl;
      }
    }
  }
}

void handleClient(ConnSocket clientSocket) {
  if (!clientSocket.is_valid()) {
    std::cerr << "Invalid socket descriptor for new client\n";
    return;
  }

  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.push_back(clientSocket);
    std::cout << "Client connected with fd: " << clientSocket.sockfd << std::endl;
  }

  std::string buffer;
  while (true) {
    if (!clientSocket.read(buffer, 1024)) {
      std::cerr << "Error receiving data on fd: " << clientSocket.sockfd << "\n";
      break;
    }

    std::cout << "Received message from client with fd " << clientSocket.sockfd << ": " << buffer << std::endl;
    broadcastMessage(buffer, clientSocket);
  }

  clientSocket.close();
  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
    std::cout << "Client removed with fd: " << clientSocket.sockfd << std::endl;
  }
}

int main() {
  ConnSocket serverSocket;

  if (!serverSocket.create_server_socket(PORT)) {
    std::cerr << "Error creating server socket\n";
    return 1;
  }

  std::cout << "Server started and listening on port " << PORT << std::endl;

  while (true) {
    ConnSocket clientSocket = serverSocket.accept_connection();
    if (!clientSocket.is_valid()) {
      std::cerr << "Error accepting connection\n";
      continue;
    }

    std::cout << "New client connection accepted\n";
    std::thread(handleClient, std::move(clientSocket)).detach();
  }

  return 0;
}
