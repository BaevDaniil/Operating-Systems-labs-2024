#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm> // For std::remove
#include <unistd.h>  // For getpid()
#include "conn_socket.hpp"

const int PORT = 12345;
std::vector<ConnSocket> clients;
std::mutex clients_mutex;

void handleClient(ConnSocket clientSocket, pid_t hostPid) {
  std::string message;
  const size_t max_size = 1024;

  while (true) {
    if (!clientSocket.read(message, max_size)) {
      std::cerr << "Error receiving message\n";
      break;
    }

    // Extract the sender PID from the message
    size_t pos = message.find(':');
    if (pos != std::string::npos) {
      pid_t senderPid = std::stoi(message.substr(0, pos));
      std::string actualMessage = message.substr(pos + 1);

      if (senderPid != hostPid) {
        std::cout << ">>> " << actualMessage << std::endl;
      }
    }
  }

  clientSocket.close();
  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
  }
}

void writeToClients(ConnSocket &serverSocket, pid_t hostPid) {
  std::string message;

  while (true) {
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    std::string fullMessage = std::to_string(hostPid) + ":" + message;

    std::lock_guard<std::mutex> lock(clients_mutex);
    for (auto &client : clients) {
      if (client.is_valid()) {
        if (!client.write(fullMessage)) {
          std::cerr << "Error sending message to client\n";
        }
      }
    }
    std::cout << "<<< " << message << std::endl;
  }
}

int main() {
  pid_t pid = getpid();

  ConnSocket serverSocket;
  if (!serverSocket.create_server_socket(PORT)) {
    std::cerr << "Error creating server socket\n";
    return 1;
  }

  std::cout << "Server started on port " << PORT << " with PID: " << pid << std::endl;

  std::thread writer(writeToClients, std::ref(serverSocket), pid);

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

    std::thread(handleClient, std::move(clientSocket), pid).detach();
  }

  writer.join();
  return 0;
}
