#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include "conn_socket.hpp"

const std::string SERVER_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 12345;

void receive_messages(Conn &clientConn) {
  std::string buffer;
  while (true) {
    if (!clientConn.read(buffer, 1024)) {
      std::cerr << "Error: connection lost.\n";
      break;
    }
    std::cout << "Message from server: " << buffer << std::endl;
  }
}

int main() {
  ConnSocket clientConn;

  if (!clientConn.connect_to_server(SERVER_ADDRESS, SERVER_PORT)) {
    std::cerr << "Failed to connect to server\n";
    return 1;
  }

  std::cout << "Connected to server " << SERVER_ADDRESS << " on port " << SERVER_PORT << std::endl;

  std::thread receiverThread(receive_messages, std::ref(clientConn));

  std::string message;
  while (true) {
    std::cout << "Enter message: ";
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    if (!clientConn.write(message)) {
      std::cerr << "Error sending message\n";
      break;
    }
  }

  clientConn.close();
  receiverThread.join();

  return 0;
}
