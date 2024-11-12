#include <iostream>
#include <string>
#include "conn_socket.hpp"

const std::string SERVER_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 12345;

int main() {
  ConnSocket clientSocket;

  if (!clientSocket.connect_to_server(SERVER_ADDRESS, SERVER_PORT)) {
    std::cerr << "Failed to connect to server\n";
    return 1;
  }

  std::string message;
  while (true) {
    std::cout << "Enter message: ";
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    if (!clientSocket.write(message)) {
      std::cerr << "Error sending message\n";
      break;
    }
  }

  clientSocket.close();
  return 0;
}
