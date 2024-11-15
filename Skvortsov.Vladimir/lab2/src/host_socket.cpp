#include <unistd.h>
#include <string>
#include <iostream>
#include <thread>
#include "conn_socket.hpp"

const int PORT = 8080;

void read_socket(ConnSocket& client_socket) {
  std::string msg;
  const size_t max_size = 1024;

  while (true) {
    if (client_socket.read(msg, max_size)) {
      std::cout << ">>> " << msg << std::endl;
    } else {
      std::cerr << "Client disconnected or error occurred\n";
      break;
    }
  }
};

void write_socket(ConnSocket& client_socket) {
  std::string msg;

  while (true) {
    std::getline(std::cin, msg);

    if (msg == "exit") {
      break;
    }

    if (!client_socket.write(msg)) {
      std::cerr << "Error sending message to client\n";
      break;
    }
    std::cout << "<<< " << msg << std::endl;
  }
};

int main() {
  pid_t pid = getpid();

  ConnSocket server_socket;
  if (!server_socket.create_server_socket(PORT)) {
    std::cerr << "Error creating server socket\n";
    return 1;
  }

  std::cout << "Server started on port " << PORT << " with PID: " << pid << std::endl;

  ConnSocket client_socket = server_socket.accept_connection();
  if (!client_socket.is_valid()) {
    std::cerr << "Failed to accept a connection\n";
    return 1;
  }

  std::thread reader(read_socket, std::ref(client_socket));
  std::thread writer(write_socket, std::ref(client_socket));

  reader.join();
  writer.join();

  client_socket.close();

  return 0;
};
