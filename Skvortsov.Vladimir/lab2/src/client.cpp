#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include "conn_socket.hpp"

const std::string SERVER_ADDRESS = "127.0.0.1";
const int SERVER_PORT = 12345;

void receiveMessages(ConnSocket& clientSocket) {
  char buffer[1024];
  while (true) {
    memset(buffer, 0, sizeof(buffer));
    ssize_t bytesReceived = clientSocket.Read(buffer, sizeof(buffer));
    if (bytesReceived < 0) {
      std::cerr << "Error: connection lost. Error code: " << strerror(errno) << "\n";
      break;
    } else if (bytesReceived == 0) {
      std::cerr << "Connection closed by server\n";
      break;
    }
    std::cout << "Message from server: " << buffer << std::endl;
  }
};

int main() {
  ConnSocket clientSocket;

  if (!clientSocket.ConnectToServer(SERVER_ADDRESS, SERVER_PORT)) {
    std::cerr << "Failed to connect to server\n";
    return 1;
  }

  std::cout << "Connected to server " << SERVER_ADDRESS << " on port " << SERVER_PORT << std::endl;

  std::thread receiverThread(receiveMessages, std::ref(clientSocket));

  std::string message;
  while (true) {
    std::cout << "Enter message: ";
    std::getline(std::cin, message);

    if (message == "exit") {
      break;
    }

    if (!clientSocket.Write(message.c_str(), message.size())) {
      std::cerr << "Error sending message\n";
      break;
    }
  }

  clientSocket.Close();
  receiverThread.join();

  return 0;
};
