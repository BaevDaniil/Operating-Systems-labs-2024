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

void handle_client(ConnSocket socket_client, pid_t host_pid) {
  std::string message;
  const size_t max_size = 1024;

  while (true) {
    if (!socket_client.read(message, max_size)) {
      std::cerr << "Error receiving message\n";
      break;
    }

    // Extract the sender PID from the message
    size_t pos = message.find(':');
    if (pos != std::string::npos) {
      pid_t senderPid = std::stoi(message.substr(0, pos));
      std::string actual_msg = message.substr(pos + 1);

      if (senderPid != host_pid) {
        std::cout << ">>> " << actual_msg << std::endl;
      }
    }
  }

  socket_client.close();
  {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), socket_client), clients.end());
  }
}

void writeToClients(ConnSocket &serverSocket, pid_t host_pid) {
  std::string message;

  while (true) {
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    std::string fullMessage = std::to_string(host_pid) + ":" + message;

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
    ConnSocket socket_client = serverSocket.accept_connection();
    if (!socket_client.is_valid()) {
      std::cerr << "Error accepting connection\n";
      continue;
    }

    {
      std::lock_guard<std::mutex> lock(clients_mutex);
      clients.push_back(socket_client);
    }

    std::thread(handle_client, std::move(socket_client), pid).detach();
  }

  writer.join();
  return 0;
}
