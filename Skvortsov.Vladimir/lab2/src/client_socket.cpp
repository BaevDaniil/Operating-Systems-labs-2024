#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include "conn_socket.hpp"

const std::string SERVER_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 8080;

void read_socket(ConnSocket& server_socket) {
  std::string msg;
  const size_t max_size = 1024;

  while (true) {
    if (server_socket.read(msg, max_size)) {
      std::cout << ">>> " << msg << std::endl;
    } else {
      std::cerr << "Server disconnected or error occurred\n";
      break;
    }
  }
}

int main() {
  pid_t pid = getpid();

  ConnSocket client_socket;
  if (!client_socket.connect_to_server(SERVER_ADDRESS, SERVER_PORT)) {
    std::cerr << "Error connecting to server\n";
    return 1;
  }

  std::cout << "Connected to server at " << SERVER_ADDRESS << ":" << SERVER_PORT << " with PID: " << pid << std::endl;

  std::thread reader(read_socket, std::ref(client_socket));

  std::string msg;
  while (true) {
    std::getline(std::cin, msg);

    if (msg == "exit") {
      break;
    }

    if (!client_socket.write(msg)) {
      std::cerr << "Error sending message to server\n";
      break;
    }
    std::cout << "<<< " << msg << std::endl;
  }

  reader.join();

  client_socket.close();

  return 0;
}
