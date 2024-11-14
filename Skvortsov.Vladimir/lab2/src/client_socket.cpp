#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>  // For getpid()
#include "conn_socket.hpp"

const std::string SERVER_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 12345;

void read_socket(ConnSocket& socket_client, pid_t client_pid) {
  std::string msg;
  const size_t max_size = 1024;

  while (true) {
    if (!socket_client.read(msg, max_size)) {
      std::cout << ">>> " << msg << std::endl;
    }
  }
}

void write_socket(ConnSocket &socket_client, pid_t client_pid) {
  std::string msg;

  while (true) {
    std::getline(std::cin, msg);

    if (msg == "exit") {
      break;
    }

    if (!socket_client.write(msg)) {
      std::cerr << "Error sending message\n";
      break;
    }
    std::cout << "<<< " << msg << std::endl;
  }
}

int main() {
  pid_t pid = getpid();

  ConnSocket socket_client;
  if (!socket_client.connect_to_server(SERVER_ADDRESS, SERVER_PORT)) {
    std::cerr << "Failed to connect to server\n";
    return 1;
  }

  std::cout << "Connected to server at " << SERVER_ADDRESS << ":" << SERVER_PORT << " with PID: " << pid << std::endl;

  std::thread reader(read_socket, std::ref(socket_client), pid);
  std::thread writer(write_socket, std::ref(socket_client), pid);

  reader.join();
  writer.join();

  return 0;
}
