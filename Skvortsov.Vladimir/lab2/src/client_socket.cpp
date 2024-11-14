#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>  // For getpid()
#include "conn_socket.hpp"

const std::string SERVER_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 12345;

void read_socket(ConnSocket &clientSocket, pid_t clientPid) {
  std::string message;
  const size_t max_size = 1024;

  while (true) {
    if (!clientSocket.read(message, max_size)) {
      std::cerr << "Error receiving message\n";
      break;
    }

    size_t pos = message.find(':');
    if (pos != std::string::npos) {
      pid_t senderPid = std::stoi(message.substr(0, pos));
      std::string actualMessage = message.substr(pos + 1);

      if (senderPid != clientPid) {
        std::cout << ">>> " << actualMessage << std::endl;
      }
    }
  }
}

void write_socket(ConnSocket &clientSocket, pid_t clientPid) {
  std::string message;

  while (true) {
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    std::string fullMessage = std::to_string(clientPid) + ":" + message;
    if (!clientSocket.write(fullMessage)) {
      std::cerr << "Error sending message\n";
      break;
    }
    std::cout << "<<< " << message << std::endl;
  }
}

int main() {
  pid_t pid = getpid();

  ConnSocket clientSocket;
  if (!clientSocket.connect_to_server(SERVER_ADDRESS, SERVER_PORT)) {
    std::cerr << "Failed to connect to server\n";
    return 1;
  }

  std::cout << "Connected to server at " << SERVER_ADDRESS << ":" << SERVER_PORT << " with PID: " << pid << std::endl;

  std::thread reader(read_socket, std::ref(clientSocket), pid);
  std::thread writer(write_socket, std::ref(clientSocket), pid);

  reader.join();
  writer.join();

  return 0;
}
